#include <iostream>
#include <cstdlib>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>

const int BUFFER_SIZE = 10;

using namespace std;

struct SharedBuffer {
    int buffer[BUFFER_SIZE];
    int count;
};

void process1(SharedBuffer* shared, sem_t* sem_cons, sem_t* sem_file) {
    sem_wait(sem_cons);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        int value = shared->buffer[i];
        cout << "Process 1 прочитал: " << value << endl;
    }
    sem_post(sem_file);
}

void process2(SharedBuffer* shared, sem_t* sem_cons, sem_t* sem_file) {
    ofstream outFile("output.txt");
    sem_wait(sem_file);
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        int value = shared->buffer[i];
        outFile << value << endl;
    }
    outFile.close();
}

int main() {
    int shm_id = shmget(IPC_PRIVATE, sizeof(SharedBuffer), IPC_CREAT | 0666);
    SharedBuffer* shared = (SharedBuffer*)shmat(shm_id, nullptr, 0);
    shared->count = 0;

    sem_t* sem_cons = sem_open("/sem_cons", O_CREAT | O_EXCL, 0666, 0);
    sem_t* sem_file = sem_open("/sem_file", O_CREAT | O_EXCL, 0666, 0);
       
    pid_t pid1 = fork();
    if (pid1 == 0) {
        process1(shared, sem_cons, sem_file);
        return 0;
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        process2(shared, sem_cons, sem_file);
        return 0;
    }

    for (int i = 0; i < BUFFER_SIZE; ++i) {
        shared->buffer[shared->count++] = rand() % 100;
        cout << "Main записал: " << shared->buffer[shared->count - 1] << endl;
    }
    sem_post(sem_cons);

    waitpid(pid1, nullptr, 0);
    waitpid(pid2, nullptr, 0);

    sem_close(sem_cons);
    sem_close(sem_file);
    sem_unlink("/sem_cons");
    sem_unlink("/sem_file");
    shmdt(shared);
    shmctl(shm_id, IPC_RMID, nullptr);
    return 0;
}

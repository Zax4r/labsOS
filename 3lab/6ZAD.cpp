#include <iostream>
#include <cstdio>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <semaphore.h>
#include <string>
#include <fcntl.h>
using namespace std;
const int N=5;
int state[N];
sem_t s[N+1];
sem_t writer;
void eat()
{
    for (int i=0;i<400;i++)
        i++;
}

void test(int i)
{
    if(state[i]==1 && state[(i+N-1)%N]!=2 && state[(i+1)%N]!=2)
    {
        state[i]=2;
        sem_post(&s[i]);
    }
}

void put_forks(int i)
{
    sem_wait(&s[N]);
    state[i]=0;
    test((i+N-1)%N);
    test((i+1)%N);
    sem_post(&s[N]);
}

void take_forks(int i)
{
    sem_wait(&s[N]);
    state[i]=1;
    test(i);
    sem_post(&s[N]);
    sem_wait(&s[i]);
}



void philosopher(int j)
{
    for (int i=1;i<1001;i++)
    {
        if (!(i%200))
        {
            take_forks(j);
            sem_wait(&writer);
            cout << "Philosopher " << j << " is eating." << endl;
            sem_post(&writer);
            eat();
            put_forks(j);
            sem_wait(&writer);
            cout << "Philosopher " << j<< " finished eating." << endl;
            sem_post(&writer);
        }
    }
    sem_wait(&writer);
    cout<<"Philosopher "<< j<< " Ended work" << endl;
    sem_post(&writer);
}

int main()
{
for (int i = 0; i < N; i++) {
        state[i] = 0;
        sem_init(&s[i], 1, 0);
    }
    sem_init(&s[N], 1, 1);
    sem_init(&writer,1,1);
    pid_t pid[N];
    for (int i = 0; i < N; i++) {
        pid[i] = fork();
        if (pid[i] == 0) {
            philosopher(i);
            return 0;
        }
    }

    for (int i = 0; i < N; i++) {
        waitpid(pid[i], nullptr, 0);
    }

    for (int i = 0; i < N; i++) {
        sem_destroy(&s[i]);
    }
    sem_destroy(&s[N]);

    return 0;
}

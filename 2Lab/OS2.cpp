#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


using namespace std;

long double taylor_sin(double x, int n) {
    long double term = x;
    long double sum = term;

    for (int i = 1; i < n; ++i) {
        term *= -x * x / (2 * i * (2 * i + 1)); //рекурентная формула
        sum += term;
    }
    return sum;
}

void calculate_sin(int i, int K, int N, int n) {
    long double x = (2 * M_PI * i) / N;
    long double value = taylor_sin(x, n);
    
    cout << "PID: " << getpid() << ", y(" << i << ") = " << value << endl;
    
        ofstream outfile("results.txt", ios::app);
    if (outfile.is_open()) {
        outfile << "y(" << i << ") = " << value << std::endl;
        outfile.close();
    } else {
        cerr<<"Trouble with file\n";
    }
}

int main() {
    int K, N, n;

    cout << "Enter the value of K (number of values to calculate): ";
    cin >> K;
    cout << "Enter the value of N: ";
    cin >> N;
    cout << "Enter the number of terms in Taylor series (n): ";
    cin >> n;

    vector<pid_t> pids(K);

    for (int i = 0; i < K; ++i) {
        pids[i] = fork(); // Создаем новый процесс

        if (pids[i] < 0) {
            cerr << "Fork failed!" << endl;
            return 1;
        } else if (pids[i] == 0) {
            // Это дочерний процесс
            calculate_sin(i, K, N, n);
            exit(0); // Завершаем дочерний процесс
        }
    }

    // Ожидаем завершения всех дочерних процессов
    for (int i = 0; i < K; ++i) {
        waitpid(pids[i], nullptr, 0);
    }

    cout << "All calculations are done." << endl;
    return 0;
}

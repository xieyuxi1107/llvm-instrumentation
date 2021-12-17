// CPP program to demonstrate multithreading
// using three different callables.
#include <iostream>
#include <thread>
#include <vector>
#include <fstream>
#include <functional>
using namespace std;

#define NUM_THREADS 1
#define ITERATIONS 100000

// A dummy function
void trace()
{
    for (unsigned int i = 0; i < ITERATIONS; ++i) {
        ifstream rfile("/proc/MockFStrace");
        rfile.get();
        rfile.close();
    }
}

void notrace()
{
    for (unsigned int i = 0; i < ITERATIONS; ++i) {
        ifstream rfile("/proc/MockFSnotrace");
        rfile.get();
        rfile.close();
    }
}

int main()
{
    vector<thread> threads;
    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        // thread th(trace);
        threads.emplace_back(notrace);
    }

    for (unsigned int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }

    return 0;
}
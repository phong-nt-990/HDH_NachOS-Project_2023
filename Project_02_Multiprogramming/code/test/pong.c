/*
    Test semaphore and multithread
    print 1000 character 'B'
*/

#include "syscall.h"

int main() {
    int i;
    for (i = 0; i < 1000; ++i) {
        // Signal("ping");
        PrintString("B");
        // Wait("pong");
    }
    Exit(0);
}

#include "syscall.h"
int main()
{
    char buffer[200];
    char *filename1;
    char *filename2;
    int id1, size1, id2, size2, c1, c2, createResult;
    PrintString("============ COPY PROGRAM =====
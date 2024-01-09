#include "syscall.h"
int main()
{
    char buffer[200];
    char *filename1;
    char *filename2;
    int id1, size1, id2, size2, c1, c2, createResult;
    PrintString("============ COPY PROGRAM ============\n\n");
    PrintString("Enter file name to copy: ");
    Read(filename1, 200, 0);
    id1 = Open(filename1, 0);
    if (id1 == -1)
    {
        PrintString("Open ");
        PrintString(filename1);
        PrintString(" error.\n");
    }
    else
    {
        size1 = Read(buffer, 200, id1);
        // PrintString("File content: \n");
        // PrintString(buffer);
        // PrintString("\n");
        c1 = Close(id1);
    }
    PrintString("Enter file name to paste: ");
    Read(filename2, 200, 0);
    createResult = Create(filename2);
    if (createResult == -1)
    {
        PrintString("Copy to ");
        PrintString(filename2);
        PrintString(" error. It's maybe exist.\n");
    }

    else
    {
        id2 = Open(filename2, 0);
        if (id2 == -1)
        {
            PrintString("Open ");
            PrintString(filename2);
            PrintString(" error.\n");
        }
        else
        {
            size2 = Write(buffer, size1, id2);
            c2 = Close(id2);
            PrintString("Copy file successfully.\n");
        }
    }

    PrintString("\n=============== EXIT PROGRAM ===============\n");
    //  //PrintString(buffer);
    Halt();
}
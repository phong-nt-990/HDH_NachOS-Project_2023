#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"
int main()
{
    char buffer[100];
    char *filename1;
    char *filename2;
    int id1, id2,size, seek,c1,c2 ,size_w;
    PrintString("=================== CONCATENATE PROGRAM =================== \n\n");
    PrintString("Enter file name to append: ");
    Read(filename1,100,0); 
    id1 = Open(filename1,0);
    if(id1==-1 )
    {
        PrintString("Open ");
        PrintString(filename1);
        PrintString(" error.\n");
    }
    else {
        seek = Seek(-1,id1);
        PrintString("Move cursor to end of file 1 successfully\n\n");
    }
    
    PrintString("Enter file name to copy: ");
    Read(filename2,200,0); 
    id2 = Open(filename2,0);
    if(id2==-1 )
    {
        PrintString("Open ");
        PrintString(filename2);
        PrintString(" error.\n");
    }
    else {
        size = Read(buffer,200,id2);
        size_w = Write(buffer,size,id1);
        if(size_w==-1)
        {
            PrintString("Concatenate failed.\n");
        }
        else
        {
            PrintString("Concatenate successfully.\n");
        }
    }
    c1 = Close(id1);
    c2 = Close(id2);

    PrintString("\n=============== EXIT PROGRAM ===============\n");
    Halt();
    return 0;
}
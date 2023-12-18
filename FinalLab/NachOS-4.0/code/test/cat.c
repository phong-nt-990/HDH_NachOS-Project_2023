#include "syscall.h"

int main()
{
    char buffer[200];
    char *filename;
    int id,c,size; 
    PrintString("================== CAT PROGRAM FILE NAME ==================\n\n");
    PrintString("Enter file name: ");
    Read(filename,100,0);
    id = Open(filename, 1);
    if(id==-1)
    {
        PrintString("Open file error.\n");
    }
    else
    {
        size = Read(buffer, 100, id);
        PrintString("File content: \n");
        PrintString("<<<< BEGIN >>>>\n");
        PrintString(buffer);
        PrintString("\n<<<< END OF FILE >>>>");
        c = Close(id);
    }

    PrintString("\n=============== EXIT PROGRAM ===============\n");
    Halt();
}
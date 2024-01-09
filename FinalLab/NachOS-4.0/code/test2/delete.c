#include "syscall.h"

int main()
{
    int id, r, c;
    char *filename;
    PrintString("================= DELETE PROGRAM ============= \n\n");
    PrintString("Enter file name: ");
    Read(filename, 200, 0);

    r = Remove(filename);
    if (r == -1)
    {
        PrintString("Remove ");
        PrintString(filename);
        PrintString(" failed.\n");
    }
    else {
        PrintString("Remove ");
        PrintString(filename);
        PrintString(" successfully.\n");
    }
    c = Close(id);

    PrintString("\n=============== EXIT PROGRAM ===============\n");
    Halt();
}
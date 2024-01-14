#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"

int main()
{
  char fileName[256];
  PrintString("============ CREATE FILE PROGRAM ============\n\n");
  PrintString("Enter file's name: ");
  Read(fileName, 255, 0);
  if (Create(fileName) == 0)
  {
    PrintString("File ");
    PrintString(fileName);
    PrintString(" created successfully!\n");
  }
  else
    PrintString("Create file failed\n");
  // int cr1 = Create("test1.txt");
  // int cr2 = Create("test2.txt");

  PrintString("\n=============== EXIT PROGRAM ===============\n");
  Halt();
  /* not reached */
}
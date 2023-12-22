#include "syscall.h"
int main()
{
    
  char buffer[1000];

  int id = Open ("open_file.c" , 1);
  int size = Read(buffer,500,id);
  int close = Close(id);
  
  Halt();
  /* not reached */
}

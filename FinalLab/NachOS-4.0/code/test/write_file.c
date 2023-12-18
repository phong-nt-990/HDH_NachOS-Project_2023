#include "syscall.h"
int main()
{

  char *buffer = "tabaopoke";
  int id = Open("hello.txt" , 0) ;
  int size = Write(buffer,3,id);
  int close = Close(id);

  Halt();
  /* not reached */
}

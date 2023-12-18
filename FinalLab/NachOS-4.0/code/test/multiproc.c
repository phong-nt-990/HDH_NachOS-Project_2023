#include "syscall.h"

int main()
{
    SpaceId newProc1;
    SpaceId newProc2;

    newProc1 = Exec("add1"); // Project 01
    newProc2 = Exec("add2"); // Project 01

    Join(newProc1);
    Join(newProc2);

  Halt();
  /* not reached */
}







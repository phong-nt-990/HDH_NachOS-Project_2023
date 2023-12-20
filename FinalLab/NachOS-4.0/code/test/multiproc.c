#include "syscall.h"

int main()
{
    SpaceId newProc1;
    SpaceId newProc2;

    newProc1 = Exec("proc01"); // Project 01
    newProc2 = Exec("proc02"); // Project 01

    Join(newProc1);
    Join(newProc2);

  Halt();
  /* not reached */
}







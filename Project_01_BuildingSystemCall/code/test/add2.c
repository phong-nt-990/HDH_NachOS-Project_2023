#include "syscall.h"

int main()
{
  int result;

  result = Add(42, 24);

  Halt();
  /* not reached */
}


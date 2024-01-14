#include "syscall.h"

int main()
{
    
  int id1 = Open ("add.c" , 0 );
  int id2 = Open ("read_file.c" , 0 );
  
  // int id2 = Open ("hello.txt" , 0 );
  // int id2 = Open ("hello.txt" , 1 ) ;
  // int id3 = Open ("hello.txt" , 2) ;
  // int seek = Seek (13 , id1) ;
  int c1 = Close(id1);
  int c2 = Close(id2);
  // int r = Remove("hello.txt");
  // int c2 = Close(id2);
  // char* buffer[100];

  // int r1 = Read(buffer, 5, id1);

  // int c1 = Close(id1);
  // int c1 = Close(id1);
  // int c2 = Close(id2);
  // int c3 = Close(id3);

  // int id4 = Open ("hello.txt" , 0 ) ;
  // int id5 = Open ("hello.txt" , 1 ) ;
  // int id6 = Open ("hello.txt" , 2) ;

  // int c4 = Close(id4);
  // int c5 = Close(id5);
  // int c6 = Close(id6);

  // int seek = Seek (3 , id) ;
  // int close = Close(id);
  // int id2 = Open ("createfisle.c" , 1) ;
  Halt();
  /* not reached */
}

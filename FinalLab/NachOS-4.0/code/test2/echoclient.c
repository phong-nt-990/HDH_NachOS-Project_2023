#include "syscall.h"

int main()
{
    char buffer1[200];
    char buffer2[200];
    char buffer3[200];
    char buffer4[200];
    char message[200];
    int port = 12345;
    int id1, id2, id3, id4, value1, value2, value3, value4, s1, s2, s3, s4;
    int receive1, receive2, receive3, receive4;
    int closeid1, closeid2, closeid3, closeid4;

    PrintString("============ ECHOCLIENT PROGRAM ============\n\n");

    id1 = SocketTCP();
    id2 = SocketTCP();
    id3 = SocketTCP();
    id4 = SocketTCP();

    PrintString("Socket 1");
    if (id1 != -1)  PrintString(" created successfully.\n");
    else            PrintString(" created failed.\n");

    PrintString("Socket 2");
    if (id2 != -1)  PrintString(" created successfully.\n");
    else            PrintString(" created failed.\n");

    PrintString("Socket 3");
    if (id3 != -1)  PrintString(" created successfully.\n");
    else            PrintString(" created failed.\n");

    PrintString("Socket 4");
    if (id4 != -1)  PrintString(" created successfully.\n");
    else            PrintString(" created failed.\n");

    PrintString("\nServer IP: ");
    PrintString("127.0.0.1");
    PrintString("\nServer port: ");
    PrintNum(port);

    value1 = Connect(id1, "127.0.0.1", 12345);
    value2 = Connect(id2, "127.0.0.1", 12345);
    value3 = Connect(id3, "127.0.0.1", 12345);
    value4 = Connect(id4, "127.0.0.1", 12345);
    
    PrintString("\n\nSocket 1");
    if (value1 != -1)  PrintString(" connected successfully.\n");
    else               PrintString(" connected failed.\n");

    PrintString("Socket 2");
    if (value2 != -1)  PrintString(" connected successfully.\n");
    else               PrintString(" connected failed.\n");

    PrintString("Socket 3");
    if (value3 != -1)  PrintString(" connected successfully.\n");
    else               PrintString(" connected failed.\n");

    PrintString("Socket 4");
    if (value4 != -1)  PrintString(" connected successfully.\n");
    else               PrintString(" connected failed.\n");

    if (value1 == -1 && value2 == -1 && value3 == -1 && value4 == -1)
        Halt();

    PrintString("\nInput message: ");
    Read(message, 200, 0);
    PrintString("\n");

    s1 = Send(id1, message, 200);
    s2 = Send(id2, message, 200);
    s3 = Send(id3, message, 200);
    s4 = Send(id4, message, 200);

    receive1 = Receive(id1, buffer1, 200);
    receive2 = Receive(id2, buffer2, 200);
    receive3 = Receive(id3, buffer3, 200);
    receive4 = Receive(id4, buffer4, 200);

    closeid1 = Close_Socket(id1);
    closeid2 = Close_Socket(id2);
    closeid3 = Close_Socket(id3);
    closeid4 = Close_Socket(id4);

    PrintString("Socket 1 sent message: ");
    if (s1 != -1)      PrintString("OK\n");
    else               PrintString("ERROR.\n");
    

    PrintString("Socket 2 sent message: ");
    if (s2 != -1)      PrintString("OK\n");
    else               PrintString("ERROR.\n");


    PrintString("Socket 3 sent message: ");
    if (s3 != -1)      PrintString("OK\n");
    else               PrintString("ERROR.\n");


    PrintString("Socket 4 sent message: ");
    if (s4 != -1)      PrintString("OK\n");
    else               PrintString("ERROR.\n");


    if (receive1 != -1)
    {
        PrintString("\nSocket 1 received: ");
        PrintString(buffer1);
    }
    if (receive2 != -1)
    {
        PrintString("\nSocket 2 received: ");
        PrintString(buffer2);
    }
    if (receive3 != -1)
    {
        PrintString("\nSocket 3 received: ");
        PrintString(buffer3);
    }
    if (receive4 != -1)
    {
        PrintString("\nSocket 4 received: ");
        PrintString(buffer4);
    }

    PrintString("\n\nSocket 1");
    if (closeid1 != -1)  PrintString(" closed successfully.\n");
    else                 PrintString(" closed failed.\n");

    PrintString("Socket 2");
    if (closeid2 != -1)  PrintString(" closed successfully.\n");
    else                 PrintString(" closed failed.\n");

    PrintString("Socket 3");
    if (closeid3 != -1)  PrintString(" closed successfully.\n");
    else                 PrintString(" closed failed.\n");

    PrintString("Socket 4");
    if (closeid4 != -1)  PrintString(" closed successfully.\n");
    else                 PrintString(" closed failed.\n");
    // PrintString (receive) ;

    PrintString("\n=============== EXIT PROGRAM ===============\n");

    Halt();
    /* not reached */
}

// #include "syscall.h"

// int main()
// {
//   char buffer1[100];
//   char buffer2[100];
//   char buffer3[100];
//   char buffer4[100];

//   int id1 = SocketTCP();
//   int id2 = SocketTCP();
//   int id3 = SocketTCP();
//   int id4 = SocketTCP();

//   int value1 = Connect(id1,"127.0.0.1",12345);
//   int value2 = Connect(id2,"127.0.0.1",12345);
//   int value3 = Connect(id3,"127.0.0.1",12345);
//   int value4 = Connect(id4,"127.0.0.1",12345);
  
//   int s1 = Send(id1, "thu nghiem 1." , 100) ;
//   int s2 = Send(id2, "thu nghiem 2." , 100) ;
//   int s3 = Send(id3, "thu nghiem 3." , 100) ;
//   int s4 = Send(id4, "thu nghiem 4." , 100) ;
  
//   int receive1 = Receive(id1 , buffer1 , 100 ) ;
//   int receive2 = Receive(id2 , buffer2 , 100 ) ;
//   int receive3 = Receive(id3 , buffer3 , 100 ) ;
//   int receive4 = Receive(id4 , buffer4 , 100 ) ;

//   int closeid1 = Close_Socket(id1);
//   int closeid2 = Close_Socket(id2);
//   int closeid3 = Close_Socket(id3);
//   int closeid4 = Close_Socket(id4);



//   //PrintString (receive) ;
//   Halt();
//   /* not reached */
// }

// int main()
// {
//   char fileName[256];
//   PrintString("============ CREATE FILE PROGRAM ============\n");
//   PrintString("Enter file's name: ");
//   Read(fileName, 255, 0);

//   if (Create(fileName) == 0)
//   {
//     PrintString("File ");
//     PrintString(fileName);
//     PrintString(" created successfully!\n");
//   }
//   else
//     PrintString("Create file failed\n");

//   Halt();
//   /* not reached */
// }












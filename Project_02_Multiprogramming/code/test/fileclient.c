
#include "syscall.h"

int main()
{
    char buffer[200];
    char content[200];
    char filename[200];

    int port = 12345;
    int socketID, value, s;
    int fileID;
    int outFileID;
    int receive;
    int closeid;
    int outCloseID;
    int resWrite;
    int eofindex = 0;
    int charCount = 0;

    PrintString("============ FILECLIENT PROGRAM ============\n\n");

    socketID = SocketTCP();

    PrintString("Socket");
    if (socketID != -1)
        PrintString(" created successfully.\n");
    else
        PrintString(" created failed.\n");

    PrintString("\nServer IP: ");
    PrintString("127.0.0.1");
    PrintString("\nServer port: ");
    PrintNum(port);

    value = Connect(socketID, "127.0.0.1", 12345);

    PrintString("\n\nSocket");
    if (value != -1)
        PrintString(" connected successfully.\n");
    else
        PrintString(" connected failed.\n");

    if (value == -1)
    {
        PrintString("=============== EXIT PROGRAM ===============\n");
        Halt();
    }

    PrintString("\nInput filename: ");
    Read(filename, 200, 0);

    fileID = Open(filename, 1);

    if (fileID == -1)
    {
        PrintString("ERROR: Cannot open file ");
        PrintString(filename);
        PrintString("\n=============== EXIT PROGRAM ===============\n");
        Halt();
    }

    // eofindex = Seek(-1, fileID);
    // PrintNum(eofindex);
    charCount = Read(buffer, 200, fileID);
    // int c = Close(id);
    if (charCount == 0)
    {
        PrintString("ERROR: File has no content.");
        PrintString("\n=============== EXIT PROGRAM ===============\n");
        Halt();
    }
    else if (charCount == -1)
    {
        PrintString("ERROR: Cannot read file content.");
        PrintString("\n=============== EXIT PROGRAM ===============\n");
        Halt();
    }

    closeid = Close(fileID);

    s = Send(socketID, buffer, 200);

    PrintString("Socket sent message: ");
    if (s != -1)
        PrintString("OK\n");
    else
        PrintString("ERROR.\n");

    receive = Receive(socketID, content, 200);

    if (receive != -1)
    {
        PrintString("\nInput filename to save content: ");
        Read(filename, 200, 0);
        outFileID = Create(filename);
        if (outFileID == -1)
        {
            PrintString("ERROR: Cannot create file ");
            PrintString(filename);
            PrintString("It's maybe exist.\n");
        }
        outFileID = Open(filename, 0);
        if (outFileID == -1)
        {
            PrintString("ERROR: Cannot open file ");
            PrintString(filename);
        }
        resWrite = Write(content, receive, outFileID);
        if (resWrite == -1)
        {
            PrintString("ERROR: Write to file ");
            PrintString(filename);
            PrintString("failed.\n");
        }
        // PrintString("\nSocket received: ");
        // PrintString("\n<<<<<<<< BEGIN >>>>>>>>\n");
        // PrintString(content);
        // PrintString("\n<<<<<<<< END OF CONTENT >>>>>>>>\n");
    }

    PrintString("Write to file ");
    PrintString(filename);
    PrintString(" successfully.\n");


    closeid = Close_Socket(socketID);

    PrintString("Socket");
    if (closeid != -1)
        PrintString(" closed successfully.\n");
    else
        PrintString(" closed failed.\n");

    closeid = Close(fileID);
    PrintString("\n=============== EXIT PROGRAM ===============\n");
    // PrintString (receive) ;
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

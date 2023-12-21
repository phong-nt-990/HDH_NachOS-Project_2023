/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MaxSocketTableSize 20


void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

bool doesFileExist(char* filename) {
    OpenFile* file = kernel->fileSystem->Open(filename);
    if (file) {
        // File exists
        delete file;
        return true;
    } else {
        // File doesn't exist
        return false;
    }
}

void SysCreateFile(char *filename)
{
  if (filename == NULL)
  {
    printf("\n Not enough memory in system");
    DEBUG(dbgSys, "\n Not enough memory in system");
    kernel->machine->WriteRegister(2, -1); // set value -1 cho thanh ghi 2 (thanh ghi trả về)

    // delete filename;
    return; // kết thúc chương trình
  }
  DEBUG(dbgSys, "\n Finish reading filename.");
  
  // check file is existed
  if (doesFileExist(filename))
  {
    DEBUG(dbgSys, "\n Error create file: File is existed.");
    // printf("\n Error create file: File is existed.");
    kernel->machine->WriteRegister(2, -1);
    return;
  }

  if (!kernel->fileSystem->Create(filename, 0))
  {
    printf("\n Error create file '%s'", filename);
    kernel->machine->WriteRegister(2, -1);
    // delete filename;
    return;
  }
  DEBUG(dbgSys, "\n Create file sucessfully.");
  kernel->machine->WriteRegister(2, 0);
  return;
}

int FindFreeSocketTableSlot(FD_Table *table)
{
  for (int i = 0; i < MaxSocketTableSize; i++)
  {
    if (table[i].socketID == 0)
    {
      return i;
    }
  }
  return -1;
}

void SysSocketTCP(FD_Table *table)
{
  
  int freeSocketIDIndex = -1;
  freeSocketIDIndex = FindFreeSocketTableSlot(table);
  if (freeSocketIDIndex == -1)
  {
    DEBUG(dbgSys, "Out of range");
    kernel->machine->WriteRegister(2, -1);
    return;
  }
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1)
  {
    DEBUG(dbgSys, "Cann't create socketTCP");
    kernel->machine->WriteRegister(2, -1);
    return;
  }

  table[freeSocketIDIndex].socketID = fd;
  DEBUG(dbgSys, "Add socket id to table successfully.");
  DEBUG(dbgSys, "ID: " << fd);
  kernel->machine->WriteRegister(2, fd);
  return;
}

void SysConnect(char *addressIp, int ServerPort, int socketid)
{
  int iRetval = -1;
  struct sockaddr_in remote = {0};

  remote.sin_addr.s_addr = inet_addr(addressIp);
  remote.sin_family = AF_INET;
  remote.sin_port = htons(ServerPort);
  iRetval = connect(socketid, (struct sockaddr *)&remote, sizeof(struct sockaddr_in));
  if (iRetval < 0)
  {
    DEBUG(dbgSys, "Connection Failed");
    kernel->machine->WriteRegister(2, -1);
    return;
  }
  else
  {
    DEBUG(dbgSys, "Connection successfully");
    kernel->machine->WriteRegister(2, 0);
  }
  return;
}

// int storeMem(char** argv, int argc) {
//     if (argv == NULL || argc == 0)
//         return 0;
    
//     int vir_stack = kernel->machine->ReadRegister(StackReg);
    
//     // Allocate memory for the array of pointers
//     int pointersSize = argc * sizeof(char*);
//     char** argvCopy = new char*[argc];

//     // Write the array of pointers to memory
//     kernel->machine->WriteMem(vir_stack, pointersSize, reinterpret_cast<int>(argvCopy));

//     // Write each string to memory and update the pointers
//     int stringStartAddr = vir_stack + pointersSize;
//     for (int i = 0; i < argc; i++) {
//         const char* str = argv[i];
//         int strLength = strlen(str) + 1;  // Include null terminator

//         // Allocate memory for the string and copy it to memory
//         kernel->machine->WriteMem(vir_stack + i * sizeof(char*), sizeof(char*), stringStartAddr);
//         kernel->machine->WriteMem(stringStartAddr, strLength, reinterpret_cast<int>(str));

//         // Update the pointer in argvCopy
//         argvCopy[i] = reinterpret_cast<char*>(stringStartAddr);

//         // Move the stringStartAddr to the next string
//         stringStartAddr += strLength;
//     }

//     // Cleanup: delete temporary arrays
//     delete[] argvCopy;

//     return vir_stack;
// }

// void Argv()
// {
// 	char **argv = kernel->currentThread->getArgv;
// 	int stack = storeMem(kernel->currentThread->getArgv, kernel->currentThread->getArgc);
// 	kernel->machine->WriteRegister(2, stack);
// }

// void Argc()
// {
// 	kernel->machine->WriteRegister(2, kernel->currentThread->getArgc);
// 	cout << kernel->currentThread->getArgc;
// }

// int SysCloseSocket()
// {

// }

#endif /* ! __USERPROG_KSYSCALL_H__ */
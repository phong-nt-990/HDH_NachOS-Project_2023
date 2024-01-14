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

#define BUFFER_SIZE 100

int readInt(int reg)
{
  DEBUG(dbgSys, "Reading integer number\n");

  int num = kernel->machine->ReadRegister(reg);

  return num;
}
// char **readCharsArray(int reg, int size)
// {
//     char **argv = new char *[size];
//     int listAddr = kernel->machine->ReadRegister(reg);

//     for (int i = 0; i < size; i++)
//     {
//         argv[i] = User2System(listAddr + i * BUFFER_SIZE, MAX_FILENAME_LENGTH);
//     }
//     return argv;
// }

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

bool doesFileExist(char *filename)
{
  OpenFile *file = kernel->fileSystem->Open(filename);
  if (file)
  {
    // File exists
    delete file;
    return true;
  }
  else
  {
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

int SysExec(char *name)
{
  // Input: Filename
  // Output: ProcessID của tiến trình
  if (name == NULL)
  {
    DEBUG(dbgSys, "Name can not be NULL");
    printf("Name can not be NULL");
    return -1;
  }

  // Mở program file của nachos
  OpenFile *progFile = kernel->fileSystem->Open(name);
  if (progFile == NULL)
  {
    printf("Cannot find the program with given name.\n");
    return -1;
  }
  delete progFile;
  int id = kernel->pTab->ExecUpdate(name);
  return id;
}

int SysJoin(int pid)
{
  return kernel->pTab->JoinUpdate(pid);
}

void SysExit(int ec)
{
  kernel->pTab->ExitUpdate(ec);
  kernel->currentThread->FreeSpace();
  kernel->currentThread->Finish();
}

OpenFileID SysOpen(char *filename, int type)
{
  int pid = kernel->currentThread->processID;
  return kernel->pTab->Open(pid, filename, type);
}

int SysClose(int id)
{
  int pid = kernel->currentThread->processID;
  return kernel->pTab->Close(pid, id);
}
int SysCreateSemaphore(char *name, int semVal)
{
  if (name == NULL)
  {
    DEBUG('a', "Out of memory\n");
    printf("Out of memory\n");
    return -1;
  }
  int res = kernel->semTab->Create(name, semVal);
  if (res == -1)
  {
    printf("Cannot create semaphore.\n");
    return -1;
  }
  return res;
}

int SysWait(char *name)
{
  if (name == NULL)
  {
    DEBUG('a', "Out of memory\n");
    printf("Out of memory");
    return -1;
  }
  int res = kernel->semTab->Wait(name);
  if (res == -1)
  {
    printf("Semaphore %s doesn't exist.\n", name);
    return -1;
  }
  return res;
}

int SysSignal(char *name)
{
  if (name == NULL)
  {
    DEBUG('a', "Out of memory\n");
    printf("Out of memory\n");
    return -1;
  }
  int res = kernel->semTab->Signal(name);
  if (res == -1)
  {
    printf("Semaphore %s doesn't exist.\n", name);
    return -1;
  }
  return res;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
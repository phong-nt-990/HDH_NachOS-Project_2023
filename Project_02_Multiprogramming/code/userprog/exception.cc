// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include <unistd.h>
#include <string.h>

// #include "synchconsole.h"
#define MaxFileLength 255
#define MAXCONTENTLENGTH 1000
#define FILE_MAX 20
#define MAX_CONTENT 100
#define MAX_STDIN_BUFFER_SIZE 1000
#define MAX_STDOUT_BUFFER_SIZE 1000
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in kernel->machine.h.
//----------------------------------------------------------------------

// Input: - User space address (int), Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space

// Init variable for socket
// int socketidTable[20] = {0};
// Init table of file descriptor socketid

///////////////////////////////////////////

char *User2System(int virtAddr, int limit)
{
    int i;
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1];
    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    for (i = 0; i < limit; i++)
    {
        kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// System2User: Chuyển dữ liệu từ kernel space sang user space
int System2User(int virtAddr, int len, char *buffer)
{
    if (len < 0)
        return -1;
    if (len == 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

char **readCharsArray(int reg, int size)
{
    char **argv = new char *[size];
    int listAddr = kernel->machine->ReadRegister(reg);

    for (int i = 0; i < size; i++)
    {
        argv[i] = User2System(listAddr + i * BUFFER_SIZE, MAX_FILENAME_LENGTH);
    }
    return argv;
}

// tăng địa chỉ thanh ghi PC  lên 4 byte, load instruction tiếp theo
void move_program_counter()
{
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

void ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);
    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
    switch (which)
    {
    case SyscallException:
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

            SysHalt();

            ASSERTNOTREACHED();
            break;
        }

        case SC_PrintString:
        {
            int idbuffer = kernel->machine->ReadRegister(4);
            char *content = User2System(idbuffer, MAXCONTENTLENGTH + 1);
            for (int i = 0; i < strlen(content); i++)
                kernel->synchConsoleOut->PutChar(content[i]);
            delete[] content;
            move_program_counter();
            return;
        }

        case SC_PrintNum:
        {
            int num = kernel->machine->ReadRegister(4);
            string s = std::to_string(num);
            char *buffer = new char[20];
            strcpy(buffer, s.c_str());
            for (int i = 0; i < strlen(buffer); i++)
                kernel->synchConsoleOut->PutChar(buffer[i]);
            delete[] buffer;
            move_program_counter();
            return;
        }

        case SC_Add:
        {
            DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
            int result;
            result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
                            /* int op2 */ (int)kernel->machine->ReadRegister(5));

            DEBUG(dbgSys, "Add returning with " << result << "\n");
            /* Prepare Result */
            kernel->machine->WriteRegister(2, (int)result);

            // /* Modify return point */
            // {
            // 	/* set previous programm counter (debugging only)*/
            // 	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

            // 	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
            // 	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

            // 	/* set next programm counter for brach execution */
            // 	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
            // }

            move_program_counter();
            return;
        }
        case SC_Create:
        {
            DEBUG(dbgSys, "\n SC_Create call ...");
            int virtAddr = kernel->machine->ReadRegister(4);
            DEBUG(dbgSys, "\n Reading virtual address of filename");
            char *filename;
            DEBUG(dbgSys, "\n Reading filename.");
            filename = User2System(virtAddr, MaxFileLength + 1);
            SysCreateFile(filename);
            kernel->machine->ReadRegister(2);
            delete[] filename;
            move_program_counter();
            return;
        }
        case SC_Open:
        {
            int virtAddr;
            int type;
            char *fileName;
            DEBUG(dbgSys, "\n SC_Open call ...");
            virtAddr = kernel->machine->ReadRegister(4);
            type = kernel->machine->ReadRegister(5);
            DEBUG(dbgSys, "\n Reading filename.");
            fileName = User2System(virtAddr, MaxFileLength + 1);

            if (fileName == NULL)
            {
                printf("\n Not enough memory in system");
                DEBUG(dbgSys, "\n Not enough memory in system");
                kernel->machine->WriteRegister(2, -1);
                delete[] fileName;
                break;
            }
            int freeSlot = kernel->fileSystem->FindFreeSlot();
            if (freeSlot != -1) // Chi xu li khi con slot trong
            {
                if (type == 0 || type == 1) // chi xu li khi type = 0 hoac 1
                {
                    if (kernel->fileSystem->checkIfFileOpened(fileName))
                    {
                        kernel->machine->WriteRegister(2, -1);
                        DEBUG(dbgSys, "\n File was opened.");
                        delete[] fileName;
                        move_program_counter();
                        return;
                    }

                    if ((kernel->fileSystem->table[freeSlot].fileID = kernel->fileSystem->Open(fileName, type)) != NULL) // Mo file thanh cong
                    {
                        DEBUG(dbgSys, "\n OpenID:"
                                          << " " << freeSlot);
                        kernel->machine->WriteRegister(2, freeSlot); // tra ve OpenFileID
                        kernel->fileSystem->addFilenameToOpenset(freeSlot, fileName);
                        DEBUG(dbgSys, "\n Open file successfully");
                        DEBUG(dbgSys, "\n File Name:" << fileName);
                    }
                    else
                    {
                        kernel->machine->WriteRegister(2, -1);
                        DEBUG(dbgSys, "File not found");
                    }
                }
                else // xu li stdout
                {
                    kernel->machine->WriteRegister(2, -1); // tra ve OpenFileID
                }
                delete[] fileName;
                move_program_counter();
                return;
            }
            DEBUG(dbgSys, "File Table full");
            kernel->machine->WriteRegister(2, -1);
            delete[] fileName;
            move_program_counter();
            return;
        }
        case SC_Close:
        {
            // Input id cua file(OpenFileID)
            // Output: 0: thanh cong, -1 that bai
            int fid = kernel->machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
            if (fid >= 0 && fid <= FILE_MAX)            // Chi xu li khi fid nam trong [0, 20]
            {
                if (kernel->fileSystem->Close(fid))
                {
                    DEBUG(dbgSys, "Close file successfully.");
                    kernel->machine->WriteRegister(2, 0);
                    break;
                }
            }
            DEBUG(dbgSys, "Close file error");
            kernel->machine->WriteRegister(2, -1);
            move_program_counter();
            return;
        }

        case SC_Read:
        {
            int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
            int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
            int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
            int OldPos;
            int NewPos;
            char *buf;
            if (id < 0 || id >= FILE_MAX)
            {
                DEBUG(dbgSys, "Open file error: File id out of range");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }

            // Kiem tra file co ton tai khong
            if (kernel->fileSystem->table[id].fileID == NULL)
            {
                DEBUG(dbgSys, "Open file error: File not exist.");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            if (id == 0)
            {
                DEBUG(dbgSys, "Open file ok: Stdin case");
                char _stdinBuffer[MAX_STDIN_BUFFER_SIZE];
                memset(_stdinBuffer, 0, sizeof(_stdinBuffer));
                char c = kernel->synchConsoleIn->GetChar();
                int n = 0;
                while (c != ((char)10))
                {
                    _stdinBuffer[n] = c;

                    if (n > MAX_STDIN_BUFFER_SIZE)
                    {
                        DEBUG(dbgSys, "Stdin buffer is too long.");
                        kernel->machine->WriteRegister(2, -1);
                        move_program_counter();
                        return;
                    }
                    n++;
                    c = kernel->synchConsoleIn->GetChar();
                }
                DEBUG(dbgSys, "Real length: " << n << "\n");
                DEBUG(dbgSys, "Stdin content: " << _stdinBuffer << "\n");
                System2User(virtAddr, MaxFileLength + 1, _stdinBuffer);
                kernel->machine->WriteRegister(2, 0);
                move_program_counter();
                return;
            }

            else if (id == 1)
            {
                DEBUG(dbgSys, "Read file not ok: stdout case");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            OldPos = kernel->fileSystem->table[id].fileID->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
            DEBUG(dbgSys, " type: " << kernel->fileSystem->table[id].fileID->type);
            buf = User2System(virtAddr, charcount);
            if ((kernel->fileSystem->table[id].fileID->Read(buf, charcount)) > 0)
            {
                DEBUG(dbgSys, "Read successfully");
                NewPos = kernel->fileSystem->table[id].fileID->GetCurrentPos();
                System2User(virtAddr, NewPos - OldPos, buf);
                kernel->machine->WriteRegister(2, NewPos - OldPos);
                DEBUG(dbgSys, "Size: " << NewPos - OldPos);
                DEBUG(dbgSys, "File content: " << buf);
                delete[] buf;
            }
            else
            {
                DEBUG(dbgSys, "Xet truong hop doc file co noi dung la rong tra ve -1");
                kernel->machine->WriteRegister(2, -1);
                delete[] buf;
            }

            move_program_counter();
            return;
        }
        case SC_Write:
        {
            int virtAddr = kernel->machine->ReadRegister(4);  // Lay dia chi cua tham so buffer tu thanh ghi so 4
            int charcount = kernel->machine->ReadRegister(5); // Lay charcount tu thanh ghi so 5
            int id = kernel->machine->ReadRegister(6);        // Lay id cua file tu thanh ghi so 6
            int OldPos;
            int NewPos;
            char *buf;
            if (id < 0 || id >= FILE_MAX)
            {
                DEBUG(dbgSys, "Error: Out of range.");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            if (id == 0)
            {
                DEBUG(dbgSys, "Write file not ok: stdin case");
                kernel->machine->WriteRegister(2, -1);
                // delete[] buf;
                move_program_counter();
                return;
            }
            else if (id == 1)
            {
                DEBUG(dbgSys, "Write file ok: stdout case");
                char *stdOutBuffer;
                stdOutBuffer = User2System(virtAddr, MAX_STDOUT_BUFFER_SIZE);
                int bufferLength = 0;

                if (stdOutBuffer)
                {
                    bufferLength = strlen(stdOutBuffer);
                }

                kernel->synchConsoleOut->PutString(stdOutBuffer, bufferLength);

                kernel->machine->WriteRegister(2, bufferLength);
                DEBUG(dbgSys, "\nSize: " << bufferLength);
                kernel->machine->WriteRegister(2, 0);
                move_program_counter();
                return;
            }
            if (kernel->fileSystem->table[id].fileID == NULL)
            {
                DEBUG(dbgSys, "Error: File is not exist.");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            if (kernel->fileSystem->table[id].fileID->type == 1)
            {
                DEBUG(dbgSys, "Error: Cann't write file because type is Read Only");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            OldPos = kernel->fileSystem->table[id].fileID->GetCurrentPos(); // Kiem tra thanh cong thi lay vi tri OldPos
            buf = User2System(virtAddr, charcount);                         // Copy chuoi tu vung nho User Space sang System Space voi bo dem buffer dai charcount
            if (kernel->fileSystem->table[id].fileID->type == 0)
            {
                if ((kernel->fileSystem->table[id].fileID->Write(buf, charcount)) > 0)
                {
                    // So byte thuc su = NewPos - OldPos
                    DEBUG(dbgSys, "Case with type = 0");
                    NewPos = kernel->fileSystem->table[id].fileID->GetCurrentPos();
                    kernel->machine->WriteRegister(2, NewPos - OldPos);
                    DEBUG(dbgSys, "Size: " << NewPos - OldPos);
                    delete buf;
                    move_program_counter();
                    return;
                }
            }
        }
        case SC_Seek:
        {
            int pos = kernel->machine->ReadRegister(4); // Lay vi tri can chuyen con tro den trong file
            int id = kernel->machine->ReadRegister(5);  // Lay id cua file
            // Kiem tra id cua file truyen vao co nam ngoai bang mo ta file khong
            if (id < 0 || id >= FILE_MAX)
            {
                DEBUG(dbgSys, "FILE OUT OF RANGE");
                // printf("\nKhong the seek vi id nam ngoai bang mo ta file.");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            // Kiem tra file co ton tai khong
            if (kernel->fileSystem->table[id].fileID == NULL)
            {
                DEBUG(dbgSys, "FILE NOT EXIST");
                // printf("\nKhong the seek vi file nay khong ton tai.");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            // Kiem tra co goi Seek tren console khong
            if (id == 0 || id == 1)
            {
                DEBUG(dbgSys, "CANNOT SEEK ON CONSOLE FILE");
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            // Neu pos = -1 thi gan pos = Length nguoc lai thi giu nguyen pos
            pos = (pos == -1) ? kernel->fileSystem->table[id].fileID->Length() : pos;
            if (pos > kernel->fileSystem->table[id].fileID->Length() || pos < 0) // Kiem tra lai vi tri pos co hop le khong
            {
                DEBUG(dbgSys, "Error: Position out of file.");
                // printf("\nKhong the seek file den vi tri nay.");
                kernel->machine->WriteRegister(2, -1);
            }
            else
            {
                // Neu hop le thi tra ve vi tri di chuyen thuc su trong file
                kernel->fileSystem->table[id].fileID->Seek(pos);
                DEBUG(dbgSys, "Pos: " << pos);
                kernel->machine->WriteRegister(2, pos);
            }
            move_program_counter();
            return;
        }

        case SC_Remove:
        {
            int virtAddr = kernel->machine->ReadRegister(4);
            char *RemoveFileName;
            RemoveFileName = User2System(virtAddr, MaxFileLength + 1);
            for (int i = 0; i < FILE_MAX; i++)
            {
                if (kernel->fileSystem->checkIfFileOpened(RemoveFileName))
                {
                    // The filename is found in the array
                    kernel->machine->WriteRegister(2, -1);
                    DEBUG(dbgSys, "\n File was opened, cannot remove this file.");
                    delete[] RemoveFileName;
                    move_program_counter();
                    return;
                }
            }

            if (kernel->fileSystem->Remove(RemoveFileName))
            {
                kernel->machine->WriteRegister(2, 0);
                DEBUG(dbgSys, "\n Remove file successfully.");
            }
            else
            {
                kernel->machine->WriteRegister(2, -1);
                DEBUG(dbgSys, "Remove file failed.");
            }
            move_program_counter();
            return;
        }

        case SC_SocketTCP:
        {
            SysSocketTCP(kernel->fileSystem->table);
            move_program_counter();
            return;
        }
        case SC_Connect:
        {
            char *addressIp;
            int socketid = kernel->machine->ReadRegister(4);
            int addIp = kernel->machine->ReadRegister(5);
            int ServerPort = kernel->machine->ReadRegister(6);
            addressIp = User2System(addIp, MaxFileLength + 1);
            DEBUG(dbgSys, "\nIP: " << addressIp);
            SysConnect(addressIp, ServerPort, socketid);
            delete[] addressIp;
            move_program_counter();
            return;
        }
        case SC_Send:
        {
            int socketid = kernel->machine->ReadRegister(4);
            int addContent = kernel->machine->ReadRegister(5);
            int sizeContent = kernel->machine->ReadRegister(6);
            char *contentSend;
            contentSend = User2System(addContent, MaxFileLength + 1);
            int shortRetval = -1;
            struct timeval tv;
            tv.tv_sec = 20; /* 20 Secs Timeout */
            tv.tv_usec = 0;
            if (setsockopt(socketid, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv)) < 0)
            {
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }
            shortRetval = send(socketid, contentSend, sizeContent, 0);
            if (shortRetval == -1 && (errno == EPIPE || errno == ECONNRESET))
            {
                // Server closed the connection
                // Handle the situation accordingly (e.g., reconnect or abort)
                // For demonstration, let's just set the return value to a specific value (-2)
                kernel->machine->WriteRegister(2, 0);
                move_program_counter();
                return;
            }
            delete[] contentSend;
            kernel->machine->WriteRegister(2, shortRetval);
            move_program_counter();
            return;
        }
        case SC_Receive:
        {
            int socketid = kernel->machine->ReadRegister(4);
            int addContent = kernel->machine->ReadRegister(5);
            int sizeContent = kernel->machine->ReadRegister(6);
            char *contentReceive = new char[sizeContent + 1];
            int shortRetval = -1;
            struct timeval tv;
            tv.tv_sec = 20; /* 20 Secs Timeout */
            tv.tv_usec = 0;
            if (setsockopt(socketid, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv)) < 0)
            {
                kernel->machine->WriteRegister(2, -1);
                move_program_counter();
                return;
            }

            shortRetval = recv(socketid, contentReceive, sizeContent, 0);

            if (shortRetval > 0)
            {
                contentReceive[shortRetval] = '\0';
                System2User(addContent, shortRetval, contentReceive);
            }

            if (shortRetval == -1 && (errno == EPIPE || errno == ECONNRESET))
            {
                // Server closed the connection
                // Handle the situation accordingly (e.g., reconnect or abort)
                // For demonstration, let's just set the return value to a specific value (-2)
                kernel->machine->WriteRegister(2, 0);
                move_program_counter();
                return;
            }
            DEBUG(dbgSys, "Receive Message: " << contentReceive);

            delete[] contentReceive;

            kernel->machine->WriteRegister(2, shortRetval);
            move_program_counter();
            return;
        }

        case SC_Close_Socket:
        {
            int id = kernel->machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4

            int iRetval = 0;

            iRetval = close(id);
            if (iRetval >= 0)
            {
                kernel->machine->WriteRegister(2, 0);
                DEBUG(dbgSys, "Close socketTCP successfully.");
            }
            else
            {
                DEBUG(dbgSys, "Close socketTCP error");
                kernel->machine->WriteRegister(2, -1);
            }
            move_program_counter();
            return;
        }

        case SC_Exec:
        {
            char *ProgramName = NULL;
            int virtualAddr = kernel->machine->ReadRegister(4); 
            ProgramName = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1); 
            int res = SysExec(ProgramName);
            if (ProgramName != NULL)
            {
                delete[] ProgramName;
                kernel->machine->WriteRegister(2, res);
                move_program_counter();
            }
            else {
                kernel->machine->WriteRegister(2, res);
                move_program_counter();
            }
            return;
        }

        case SC_ExecV:
        {
            DEBUG(dbgSys, "SC_ExecV call ...\n");
            int argc = readInt(4);
            char **argv = readCharsArray(5, argc);
            char *progname = argv[0];

            if (!progname) return;
            DEBUG(dbgSys, "Program name : '" << progname << "'\n");

            int pid = kernel->pTab->ExecVUpdate(progname, argc, argv);
            if (pid == -1)
            {
                DEBUG(dbgSys, "Can't exec file '" << progname << "'\n");
                kernel->machine->WriteRegister(2, (int)-1);
                delete progname;
                return;
            }

            DEBUG(dbgSys, "Successfully executed file '" << progname << "'\n");
            kernel->machine->WriteRegister(2, pid); 
            delete progname;
        }

        case SC_Join:
        {
            int pid;
            pid = kernel->machine->ReadRegister(4);         
            kernel->machine->WriteRegister(2, SysJoin(pid));
            move_program_counter();
            return;
        }
        case SC_Exit:
        {
            int exitCode;
            // Đọc tham số Exitcode
            exitCode = kernel->machine->ReadRegister(4);  
            SysExit(exitCode);
            move_program_counter();
            return;
        }

        case SC_CreateSemaphore:
        {
            int virtualAddr = kernel->machine->ReadRegister(4);
            int semVal = kernel->machine->ReadRegister(5);
            char *Semname = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);
            int result = SysCreateSemaphore(Semname, semVal);
            if (Semname != NULL)
                delete[] Semname;
            kernel->machine->WriteRegister(2, result);
            move_program_counter();
            return;
        }

        case SC_Wait:
        {
            int virtualAddr = kernel->machine->ReadRegister(4);
            char *name = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);
            int result = SysWait(name);
            kernel->machine->WriteRegister(2, result);
            move_program_counter();
            return;
        }
        
        case SC_Signal:
        {
            int virtualAddr = kernel->machine->ReadRegister(4);
            char *name = User2System(virtualAddr, MAX_FILENAME_LENGTH + 1);
            int result = SysSignal(name);
            kernel->machine->WriteRegister(2, result);
            move_program_counter();
            return;
        }

        default:
            cerr << "Unexpected system call " << type << "\n";
            move_program_counter();
            break;
        }
        break;
        
    // Các exception khác được triển khai sau đây
    case PageFaultException:
        DEBUG(dbgSys, "No valid translation found\n");
        printf("No valid translation found\n");
        SysHalt();
        break;

    case ReadOnlyException:
        DEBUG(dbgSys, "Write attempted to page marked \"read-only\"\n");
        printf("Write attempted to page marked \"read-only\"\n");
        SysHalt();
        break;

    case BusErrorException:
        DEBUG(dbgSys, "Translation resulted in an invalid physical address\n");
        printf("Translation resulted in an invalid physical address\n");
        SysHalt();
        break;

    case AddressErrorException:
        DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space\n");
        printf("Unaligned reference or one that was beyond the end of the address space\n");
        SysHalt();
        break;

    case OverflowException:
        DEBUG(dbgSys, "Integer overflow in add or sub\n");
        printf("Integer overflow in add or sub\n");
        SysHalt();
        break;

    case IllegalInstrException:
        DEBUG(dbgSys, "Unimplemented or reserved instr\n");
        printf("Unimplemented or reserved instr\n");
        SysHalt();
        break;

    case NumExceptionTypes:
        DEBUG(dbgSys, "Number exception types\n");
        printf("Number Exception types\n");
        SysHalt();
        break;
    
    default:
        cerr << "Unexpected user mode exception" << which << "\n";
    break;
    }
        ASSERTNOTREACHED();
}

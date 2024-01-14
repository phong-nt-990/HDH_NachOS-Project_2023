/*  ptable.h
    Data structure for manage process
        handle system call
*/

#ifndef PTABLE_H
#define PTABLE_H

#include "bitmap.h"
#include "pcb.h"
#include "synch.h"

#define MAX_PROCESS 10

class PCB;
class Semaphore;

class PTable
{
private:
    Bitmap *bm;            // mark the locations that have been used in pcb
    PCB *pcb[MAX_PROCESS]; // Init ptable
    int psize;             // Init the size of process mem
    Semaphore *bmsem;      // Semaphore de ngan chan// used to prevent the case of loading 2 processes at the same time
public:
    // Constructor initialize the size of the PCB object to store the process
    // size. Set the initial value to null

    PTable(int size); // Constructor
    ~PTable();        // Destructor

    int ExecUpdate(char *name);            // Process the syscall SC_Exec
    void ExitUpdate(int ec);               // Process the syscall SC_Exit
    int ExecVUpdate(char *, int, char **); // Process the syscall SC_ExecV
    int JoinUpdate(int id);                // Process the syscall SC_Join

    bool IsExist(int pid);     // Check a process exist or not
    int GetFreeSlot();         // Find the free slot in PTable to save the new process information
    void Remove(int pid);      // Delete the PID from the PTable
    char *GetFileName(int id); // Return the process name

    void initStartProcess(char *ProcssName); // Start an Process

    // ============= Các hàm thao tác với file
    OpenFileID Open(int pid, char *name, int type);
    int Close(int pid, OpenFileID fid);
    int Read(int pid, char *buffer, int charcount, OpenFileID fid);
    int Write(int pid, char *buffer, int charcount, OpenFileID fid);
    int Seek(int pid, int position, OpenFileID fid);
};

#endif
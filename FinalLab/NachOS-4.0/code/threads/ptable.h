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

class PTable {
private:
    Bitmap *bm;             /// mark the locations that have been used in pcb
    PCB *pcb[MAX_PROCESS];
    int psize;
    Semaphore *bmsem;       // used to prevent the case of loading 2 processes at the same time
public:
//Constructor initialize the size of the PCB object to store the process size. Set the initial value to null
    PTable(int size);
    ~PTable(); // Destructor


    void ExitUpdate(int ec);    // Process the syscall SC_Exec
    int ExecUpdate(char *name); // Process the syscall SC_Exit
    int JoinUpdate(int id);     // Process the syscall SC_Join

    bool IsExist(int pid);      // Find the free slot in PTable to save the new process information
    int GetFreeSlot();          // Check a process exist or not
    void Remove(int pid);       // Delete the PID from the PTable
    char *GetFileName(int id);  // Tra ve ten tien trinh

    void initStartProcess(char* name);  // Return the process name
   
    OpenFileID Open(int pid, char*name, int type);  // mo file name o tien trinh pid
    int Close(int pid, OpenFileID fid); // dong file fid o tien trinh pid
    int Read(int pid, char* buffer,int charcount, OpenFileID fid);      // doc file o tien trinh pid
    int Write(int pid, char* buffer, int charcount, OpenFileID fid);    // ghi file o tien trinh pid
    int Seek(int pid, int position, OpenFileID fid);    // seek file fid o tien trinh pid
};

#endif
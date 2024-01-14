/*	pcb.h
	Process Control Block
*/

#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

const int MAX_FILENAME_LENGTH = 32;

class Semaphore;

struct ProcessArg
{
    int pid;
    int argc;
    char **argv;
};

class PCB {
   private:
    Semaphore *joinsem; // semaphore for join process
    Semaphore *exitsem; // semaphore for exit process
    Semaphore *multex;  // exclusive access semaphore

    int exitcode;
    int numwait;        // the number of join process

    Thread *thread;     // Process
    char filename[MAX_FILENAME_LENGTH]; // Process name

   public:
    int parentID;       // The parent process’s ID
    OpenFile** fileTable;   // Manage all program file in this Array
    int index;
    
    PCB();              // Constructor
    PCB(int id);        // Destructor
    ~PCB();

    int Exec(char *filename, int pid);  //Create a thread with the name is filename and the process id is pid
    int ExecV(char *name, int pid, int argc, char **argv); // Like Exec, with argument
    int GetID();	    // Return the PID of the current process
    int GetNumWait();   // Return the number of the waiting process


    void JoinWait();	// The parent process wait for the child process finishes

    void ExitWait();	// The child process finishes
    void JoinRelease();	// The child process notice the parent process
    void ExitRelease();	// The parent process accept to exit the child process


    void IncNumWait();  // Increase the number of the waiting process
    void DecNumWait();  // Decrease the number of the waiting process

    void SetExitCode(int ec);   // Set the exit code for the process
    int GetExitCode();          // Return the exitcode

    void SetFileName(char *fn); // Set the process name
    char *GetFileName();        // Return the process name

    int FindFreeSlot();         // Discover the free space in OpenFile** array.

    // ================== Các hàm với filesystem
    OpenFileID Open(char*name, int type);   
    int Close(OpenFileID fid);
    int Read(char* buffer, int charcount, OpenFileID id);   
    int Write(char* buffer, int charcount, OpenFileID id);  
    int Seek(int position, OpenFileID id);  
};

#endif
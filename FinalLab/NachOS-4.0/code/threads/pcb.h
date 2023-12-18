/*	pcb.h
	Process Control Block
*/

#ifndef PCB_H
#define PCB_H

#include "synch.h"
#include "thread.h"

const int MAX_FILENAME_LENGTH = 32;

class Semaphore;

class PCB {
private:
    Semaphore* joinsem; // semaphore for join process
    Semaphore* exitsem; // semaphore for exit process
    Semaphore* multex; // exclusive access semaphore

    Thread *thread;     // Tien trinh
    char filename[MAX_FILENAME_LENGTH]; // Ten tien trinh

    int exitcode; 
    int numwait; // the number of join process
public:
    int parentID; // The parent process’s ID
    OpenFile** fileTable; 
    int index;
    PCB();
    PCB(int id); // Constructor
    ~PCB(); // Destructor
    // Load the program has the name is “filename” and the process id is pid
    int Exec(char* name,int pid); //Create a thread with the name is filename and the process id is pid
    int GetID(); // Return the PID of the current process
    int GetNumWait(); // Return the number of the waiting process
    void JoinWait();// The parent process wait for the child process finishes
    void ExitWait(); // The child process finishes
    void JoinRelease(); // The child process notice the parent process
    void ExitRelease(); // The parent process accept to exit the child process
    void IncNumWait(); // Increase the number of the waiting process
    void DecNumWait(); // Decrease the number of the waiting process
    void SetExitCode(int); // Set the exit code for the process
    int GetExitCode(); // Return the exitcode
    void SetFileName(char*); // Set the process name
    char* GetFileName(); // Return the process name

    int FindFreeSlot();         // Tim vi tri con trong de luu file can open
    OpenFileID Open(char*name, int type);   // mo file name
    int Close(OpenFileID fid);              // dong file fid
    int Read(char* buffer, int charcount, OpenFileID id);   // doc file vao buffer
    int Write(char* buffer, int charcount, OpenFileID id);  // ghi file vao buffer
    int Seek(int position, OpenFileID id);  // seek den vi tri position cua file id
};

#endif
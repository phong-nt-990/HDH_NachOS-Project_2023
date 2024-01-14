/* stable.h
    Data structure for manage semaphore
*/

#ifndef STABLE_H
#define STABLE_H

#include "bitmap.h"
#include "synch.h"

const unsigned int MAX_SEMAPHORE = 10;

class Semaphore;

class Sem {
private:
    char *name; // The semaphore name

    Semaphore *sem; // Create semaphore for management


public:
    // Initial the Sem object, the started value is null
    // Remember to initial the Sem to use
    Sem(char *na, int i);
    ~Sem();

    char *getName();

    void wait();   // Down Process
    void signal(); // Up Process
};

class STable
{
private:
    Bitmap *mSemBitMap;
    Sem *semTable[MAX_SEMAPHORE];

public:
    STable();   
    ~STable();

    int Create(char *name, int value); // tao 1 semaphore "name" neu chua ton tai ten
    int Wait(char *name);              // semaphore "name" wait
    int Signal(char *name);            // semaphore "name" giai phong
    int FindFreeSlot();                // tim slot trong trong STable
};

#endif // SCHEDULER_H
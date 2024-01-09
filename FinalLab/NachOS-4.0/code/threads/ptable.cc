#include "ptable.h"

#include "main.h"
#include "openfile.h"

PTable::PTable(int size) {
    if (size < 0)
        return;
    // Size khong vuot qua so process toi da 
    if (size > MAX_PROCESS)
        size = MAX_PROCESS;
    psize = size;
    // Khoi tao bm va bmsem de su dung
    bm = new Bitmap(size);
    bmsem = new Semaphore("bmsem", 1);
    for (int i = 0; i < psize; ++i)
        pcb[i] = 0;
}

void PTable::initStartProcess(char* name) {
    // Danh dau tren tien trinh
    bm->Mark(0);
    pcb[0] = new PCB(0);
    pcb[0]->SetFileName(name);
    pcb[0]->parentID = -1;
}

PTable::~PTable() {
    // Giai phong bo nho bm
    if (bm != 0)
        delete bm;
    // Giai phong bo nho pcb
    for (int i = 0; i < psize; ++i) {
        if (pcb[i] != 0)
            delete pcb[i];
    }
    // Giai phing bo nho bmsem
    if (bmsem != 0)
        delete bmsem;
}

int PTable::ExecUpdate(char *name) {
    bmsem->P();
    // name phai khac NULL
    if (name == NULL) {
        printf("\nError in PTable::ExecUpdate: Name is NULL.\n");
        bmsem->V();
        return -1;
    }
    // Trung ten voi tien trinh hien tai
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        printf("\nError in PTable::Exec: Already existed.\n");
        bmsem->V();
        return -1;
    }

    // Trung ten voi main thread
    if (strcmp(name, pcb[0]->GetFileName()) == 0) {
        printf("\nError in PTable::ExecUpdate: Can not run main process");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot();
    // Khong con free slot
    if (index < 0) {
        printf("\nError in PTable::Exec: No free slot.\n");
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);
    pcb[index]->parentID = kernel->currentThread->processID;
    int pid = pcb[index]->Exec(name, index);
    bmsem->V();
    return pid;
}

int PTable::ExecVUpdate(char *filename, int argc, char **argv)
{
    bmsem->P(); // Avoid loading 2 processes at the same time.

    if (filename == NULL) // Check the validity of the program "name".
    {
        printf("PTable::ExecUpdate Error -- File name is NULL\n");
        bmsem->V();
        return -1;
    }

    OpenFile *executable = kernel->fileSystem->Open(filename);
    if (executable == NULL) // Check the existence of the program "name" by calling the Open method of FileSystem class.
    {
        printf("PTable::ExecUpdate Error -- File name is invalid\n");
        bmsem->V();
        return -1;
    }
    kernel->fileSystem->Close(executable->GetCurrentPos()); // Close the file after checking its existence.

    if (strcmp(filename, kernel->pTab->GetFileName(kernel->currentThread->processID)) == 0) // Compare program name and currentThread name to make sure this program is not called Exec itself.
    {
        printf("PTable::ExecUpdate Error -- Can't execute itself\n");
        bmsem->V();
        return -1;
    }

    int index = this->GetFreeSlot(); // Find the empty position in the PTable table
    if (index == -1)
    {
        printf("PTable::ExecUpdate Error -- No empty slot\n");
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);                                 // create a new PCB with processID being the index of this slot
    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(filename);
    pcb[index]->parentID = kernel->currentThread->processID;
    pcb[index]->ExecV(filename, index, argc, argv);
    bmsem->V();
    return index;
}


int PTable::JoinUpdate(int id) {
    // ID phai lon hon -1
    if (id < 0) {
        printf("\nError in PTable::JoinUpdate: Invalid id = %d\n", id);
        return -1;
    }
    // Chi cho phep join vao tien trinh cha 
    if (kernel->currentThread->processID != pcb[id]->parentID) {
        printf("\nError in PTable::JoinUpdate: Joining a non-parent process.\n");
        return -1;
    }
    // Tang so tien trinh cho
    pcb[pcb[id]->parentID]->IncNumWait();
    pcb[id]->JoinWait();    // cho den khi tien trinh con ket thuc
    int ec = pcb[id]->GetExitCode();
    pcb[id]->ExitRelease(); // cho phep tien trinh con duoc ket thuc

    return ec;
}

void PTable::ExitUpdate(int exitcode) {
    int id = kernel->currentThread->processID;
    // neu la main process thi Halt
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
    }
    // neu khong ton tai process id
    if (IsExist(id) == false) {
        printf("\nError in PTable::ExitUpdate: This %d is not exist. Try again?", id);
    }

    pcb[id]->SetExitCode(exitcode);
    pcb[pcb[id]->parentID]->DecNumWait();
    pcb[id]->JoinRelease(); // giai phong tien trinh
    pcb[id]->ExitWait();    // xin tien trinh cha cho exit
    Remove(id);
}

int PTable::GetFreeSlot() {
    return bm->FindAndSet();
}

bool PTable::IsExist(int pid) {
    return bm->Test(pid);
}

void PTable::Remove(int pid) {
    // Xoa tien trinh tren bitmap
    bm->Clear(pid);
    // Xoa tien trinh tren mang quan li
    if (pcb[pid] != 0)
        delete pcb[pid];
}

char *PTable::GetFileName(int pid) {
    return (pcb[pid]->GetFileName());
}

OpenFileID PTable::Open(int pid, char*name, int type) {
    return pcb[pid]->Open(name, type);
}

int PTable::Close(int pid, OpenFileID fid) {
    return pcb[pid]->Close(fid);
}

int PTable::Read(int pid, char* buffer,int charcount, OpenFileID fid) {
    return pcb[pid]->Read(buffer, charcount, fid);
}
    
int PTable::Write(int pid, char* buffer, int charcount, OpenFileID fid) {
    return pcb[pid]->Write(buffer, charcount, fid);
}

int PTable::Seek(int pid, int position, OpenFileID fid) {
    return pcb[pid]->Seek(position, fid);
}
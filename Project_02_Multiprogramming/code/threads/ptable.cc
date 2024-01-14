#include "ptable.h"

#include "main.h"
#include "openfile.h"

PTable::PTable(int size) {
    // size phai lon hon 0
    if (size < 0) return;
    // Size khong vuot qua so process toi da 
    if (size > MAX_PROCESS) size = MAX_PROCESS;
    psize = size;
    bmsem = new Semaphore("bmsem", 1);
    bm = new Bitmap(size);
    for (int i = 0; i < psize; ++i)
    {
        pcb[i] = 0;
    }
}

PTable::~PTable() {
    // Giải phóng biến bitmap
    if (bm != 0) delete bm;
    // Giải phòng biến pcb
    for (int i = 0; i < psize; ++i) {
        if (pcb[i] != 0)
            delete pcb[i];
    }
    // Giải phóng biến bmsem
    if (bmsem != 0) delete bmsem;
}

void PTable::initStartProcess(char* name) 
{
    // Gán thông tin vào PTable
    bm->Mark(0);
    pcb[0] = new PCB(0);
    pcb[0]->SetFileName(name);
    pcb[0]->parentID = -1;
}

int PTable::ExecUpdate(char *name) {
    // Tránh load 2 chương trình cùng 1 lúc
    bmsem->P();
    
    // Ten chuong trinh phai khac null
    if (name == NULL) {
        printf("\nError in PTable::ExecUpdate: Name is NULL.\n");
        DEBUG(dbgSys, "\nError in PTable::ExecUpdate: Name is NULL.\n");
        bmsem->V();
        return -1;
    }
    // So sánh tên chương trình có trùng tên với currentThread không, tránh trường
    //      hợp thực thi chính nó
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        printf("\nError in PTable::ExecUpdate: Cannot execute itself.\n");
        DEBUG(dbgSys, "\nError in PTable::ExecUpdate: Cannot execute itself.\n");
        bmsem->V();
        return -1;
    }

    // Trung ten voi main thread
    if (strcmp(name, pcb[0]->GetFileName()) == 0) {
        printf("\nError in PTable::ExecUpdate: Can not run main process");
        bmsem->V();
        return -1;
    }

    // Kiểm tra xem program name có tồn tại trong thư mục test hay không, sử dụng 
    // phương thức của lớp FileSystem
    OpenFile *executable = kernel->fileSystem->Open(name);
    if (executable == NULL)
    {
        printf("Error in PTable::ExecUpdate: File name is invalid\n");
        bmsem->V();
        return -1;
    }
    kernel->fileSystem->Close(executable->GetCurrentPos()); 
    // Đóng file khi kiểm tra xong

    // Lấy index của vị trí trống trong ptable
    int index = this->GetFreeSlot();

    if (index < 0) {
        printf("\nError in PTable::Exec: No free slot in table.\n");
        bmsem->V();
        return -1;
    }

    // new 1 pcb
    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);

    // parentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->processID;
    int pid = pcb[index]->Exec(name, index);
    bmsem->V();

    // Trả về processID
    return pid;
}

int PTable::ExecVUpdate(char *name, int argc, char **argv) // Cấu trúc hàm ExecVUpdate tương tự như ExecUpdate
{
    // Tránh load 2 chương trình cùng 1 lúc
    bmsem->P();

    // Ten chuong trinh phai khac null
    if (name == NULL)
    {
        printf("PTable::ExecVUpdate Error: File name is NULL\n");
        DEBUG(dbgSys, "\nError in PTable::ExecUpdate: Name is NULL.\n");
        bmsem->V();
        return -1;
    }

    // So sánh tên chương trình có trùng tên với currentThread không, tránh trường
    //      hợp thực thi chính nó
    if (strcmp(name, kernel->currentThread->getName()) == 0) {
        printf("\nError in PTable::ExecUpdate: Cannot execute itself.\n");
        DEBUG(dbgSys, "\nError in PTable::ExecUpdate: Cannot execute itself.\n");
        bmsem->V();
        return -1;
    }

    // Trung ten voi main thread
    if (strcmp(name, pcb[0]->GetFileName()) == 0) {
        printf("\nError in PTable::ExecUpdate: Can not run main process");
        bmsem->V();
        return -1;
    }

    // Kiểm tra xem program name có tồn tại trong thư mục test hay không, sử dụng 
    // phương thức của lớp FileSystem
    OpenFile *executable = kernel->fileSystem->Open(name);
    if (executable == NULL)
    {
        printf("Error in PTable::ExecUpdate: File name is invalid\n");
        bmsem->V();
        return -1;
    }
    kernel->fileSystem->Close(executable->GetCurrentPos()); 
    // Đóng file khi kiểm tra xong

    // Lấy index của vị trí trống trong ptable
    int index = this->GetFreeSlot();

    if (index < 0) {
        printf("\nError in PTable::ExecV: No free slot in table.\n");
        bmsem->V();
        return -1;
    }

    pcb[index] = new PCB(index);
    pcb[index]->SetFileName(name);
    
    // parentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->processID;
    int pid = pcb[index]->ExecV(name, index, argc, argv);
    bmsem->V();
    
    // Trả về processID
    return pid;
}


int PTable::JoinUpdate(int id) {
    // ProcessID phải không âm
    if (id < 0) {
        printf("\nError in PTable::JoinUpdate: Invalid id = %d\n", id);
        DEBUG(dbgSys, "\nError in PTable::JoinUpdate: Invalid id = %d\n" << id);
        return -1;
    }
    
    // Kiểm tra tiến trình muốn join vào có phải là tiến trình cha không
    if (kernel->currentThread->processID != pcb[id]->parentID) {
        printf("\nError in PTable::JoinUpdate: Can't join in process which is not it's parent process.\n");
        DEBUG(dbgSys,"\nError in PTable::JoinUpdate: Can't join in process which is not it's parent process.\n");
        return -1;
    }

    // Tăng numwait và gọi JoinWait() để chờ tiến trình con thực hiện.
	// Sau khi tiến trình con thực hiện xong, tiến trình được giải phóng
    pcb[pcb[id]->parentID]->IncNumWait();

    // Chờ đến khi tiến trình kết thúc
    pcb[id]->JoinWait();
    
    // Xử lý exitcode
    int exitcode = pcb[id]->GetExitCode();

    // Cho phép tiến trình con kết thúc
    pcb[id]->ExitRelease();

    return exitcode;
}

void PTable::ExitUpdate(int exitcode) {
    int id = kernel->currentThread->processID;
    // Nếu tiến trình đang gọi là main process thì Halt()
    if (id == 0) {
        kernel->currentThread->FreeSpace();
        kernel->interrupt->Halt();
    }
    // neu khong ton tai process id
    if (IsExist(id) == false) {
        printf("\nError in PTable::ExitUpdate: This %d is not exist.", id);
        DEBUG(dbgSys, "\nError in PTable::ExitUpdate: This %d is not exist." << id);
    }

    // Set exitcode cho tiến trình gọi
    pcb[id]->SetExitCode(exitcode);

    // Giảm numwait
    pcb[pcb[id]->parentID]->DecNumWait();
    // Giải phóng tiến trình cha
    pcb[id]->JoinRelease(); 
    // Thông báo cho tiến trình cha là exit
    pcb[id]->ExitWait();
    Remove(id);
}

int PTable::GetFreeSlot() {
    return bm->FindAndSet();
}

bool PTable::IsExist(int pid) {
    return bm->Test(pid);
}

void PTable::Remove(int pid) {
    // Clear bitmap
    bm->Clear(pid);
    // Xoá tiến trình trong ptable
    if (pcb[pid] != 0)
        delete pcb[pid];
}

// =============  Các hàm thao tác với file

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
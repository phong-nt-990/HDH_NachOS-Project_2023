#include "syscall.h"


int main()
{
    int PingPID, PongPID;
    PrintString("Ping-Pong application test\n");
    CreateSemaphore("ping", 0);
    CreateSemaphore("pong", 0);

    PingPID = Exec("ping");
    PongPID = Exec("pong");
    Join(PingPID);
    Join(PongPID);
}

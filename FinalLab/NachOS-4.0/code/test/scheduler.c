#include "syscall.h"


int main()
{
    int PingPID, PongPID;
    PrintString("Ping-Pong application test\n");
    PingPID = Exec("ping");
    PongPID = Exec("pong");
    Join(PingPID);
    Join(PongPID);
}
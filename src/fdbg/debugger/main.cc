#include "fdbg.hh"

int main()
{
    fdbg::DebuggerClient client("/dev/ttys001");
    client.set_debugging_level(fdbg::DebuggingLevel::TRACE);
    client.ack_sync(0x1234);
}
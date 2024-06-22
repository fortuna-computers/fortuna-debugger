#ifndef SRVCOMM_H_
#define SRVCOMM_H_

#include "libfdbg-server.h"

int  comm_send_message(FdbgServer* server, fdbg_ToDebugger* msg);
bool comm_receive_next_message(FdbgServer* server, fdbg_ToComputer* msg, bool* error);

#endif
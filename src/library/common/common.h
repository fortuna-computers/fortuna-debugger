#ifndef COMMON_H_
#define COMMON_H_

#include "protobuf/to-debugger.nanopb.h"

static fdbg_ReadMemoryResponse rm;
static fdbg_ComputerStatus cs;
static fdbg_BreakpointList bkp;
static fdbg_RunStatus rs;

#define EMULATOR_BAUD_RATE  921600
#define MAX_MESSAGE_SZ      2048
#define MAX_MEMORY_TRANSFER sizeof(rm.bytes)
#define MAX_BREAKPOINTS     (sizeof(bkp.addr) / sizeof(bkp.addr[0]))
#define MAX_EVENTS          (sizeof(rs.events) / sizeof(rs.events[0]))
#define MAX_EVENTS_STEP     (sizeof(cs.events) / sizeof(cs.events[0]))

#endif //COMMON_H_

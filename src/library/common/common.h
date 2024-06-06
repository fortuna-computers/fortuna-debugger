#ifndef COMMON_H_
#define COMMON_H_

#include "to-debugger.nanopb.h"

static fdbg_ReadMemoryResponse rm;
static fdbg_ComputerStatus cs;
static fdbg_BreakpointList bkp;
static fdbg_RunStatus rs;
static fdbg_Debug debug;

#define EMULATOR_BAUD_RATE  921600
#define MAX_MESSAGE_SZ      2048
#define MAX_MEMORY_TRANSFER 64
#define MAX_BREAKPOINTS     (sizeof(bkp.addr) / sizeof(bkp.addr[0]))
#define MAX_EVENTS          (sizeof(rs.events) / sizeof(rs.events[0]))
#define MAX_EVENTS_STEP     (sizeof(cs.events) / sizeof(cs.events[0]))
#define MAX_EVENTS_STATUS   (sizeof(rs.events) / sizeof(rs.events[0]))
#define STACK_SZ            (sizeof(cs.stack.bytes) / sizeof(cs.stack.bytes[0]))
#define DEBUG_SZ            (sizeof(debug.text))

#endif //COMMON_H_

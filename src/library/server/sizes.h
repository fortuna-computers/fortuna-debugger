#ifndef SIZES_H_
#define SIZES_H_

#include "to-debugger.nanopb.h"

#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))

static fdbg_ComputerStatus cs;
static fdbg_BreakpointList bkp;
static fdbg_RunStatus rs;
static fdbg_Debug debug;

#define MAX_BREAKPOINTS     (FIELD_SIZEOF(fdbg_BreakpointList, addr) / FIELD_SIZEOF(fdbg_BreakpointList, addr[0]))
#define MAX_EVENTS_STEP     (FIELD_SIZEOF(fdbg_ComputerStatus, events) / FIELD_SIZEOF(fdbg_ComputerStatus, events[0]))
#define MAX_EVENTS_STATUS   (FIELD_SIZEOF(fdbg_RunStatus, events) / FIELD_SIZEOF(fdbg_RunStatus, events[0]))
#define STACK_SZ            (FIELD_SIZEOF(fdbg_ComputerStatus, stack.bytes) / FIELD_SIZEOF(fdbg_ComputerStatus, stack.bytes[0]))
#define DEBUG_SZ            (FIELD_SIZEOF(fdbg_Debug, text))

#endif
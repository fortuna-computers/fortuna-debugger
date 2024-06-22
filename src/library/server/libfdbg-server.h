#ifndef LIBFDBG_SERVER_H_
#define LIBFDBG_SERVER_H_

#include <stdint.h>
#include <stddef.h>

#include "common.h"

#include "to-debugger.nanopb.h"
#include "to-computer.nanopb.h"

#include "sizes.h"

// runs either in the emulator or the computer

#define SERIAL_TIMEOUT 0xfffd
#define SERIAL_ERROR   0xfffe
#define SERIAL_NO_DATA 0xffff

#ifndef ADDR_TYPE
#  define ADDR_TYPE uint64_t
#endif

typedef struct FdbgServer FdbgServer;

typedef struct FdbgServerIOCallbacks {
    uint16_t (*read_byte_async)(FdbgServer* server);
    void     (*write_byte)(FdbgServer* server, uint8_t byte);
} FdbgServerIOCallbacks;

typedef struct FdbgServerEvents {
    fdbg_ComputerStatus (*get_computer_status)(FdbgServer* server);
    fdbg_CycleResponse  (*cycle)(FdbgServer* server);
    void                (*reset)(FdbgServer* server);
    ADDR_TYPE           (*step)(FdbgServer* server, bool full, fdbg_Status* status);
    bool                (*write_memory)(FdbgServer* server, uint8_t nr, ADDR_TYPE pos, uint8_t* data, uint8_t sz, ADDR_TYPE* first_failed);
    void                (*read_memory)(FdbgServer* server, uint8_t nr, ADDR_TYPE pos, uint8_t sz, uint8_t* out_data);
    void                (*run_forever)(FdbgServer* server);
    bool                (*next_instruction)(FdbgServer* server, ADDR_TYPE* addr);  // return true if next instruction is a subroutine
    void                (*on_keypress)(FdbgServer* server, const char* key);
    void                (*interrupt)(FdbgServer* server, uint64_t number);
} FdbgServerEvents;

typedef struct FdbgServer {
    uint16_t              machine_id;
    FdbgServerIOCallbacks io_callbacks;
    ADDR_TYPE             breakpoints[MAX_BREAKPOINTS];
    ADDR_TYPE             next_bkp;
    uint32_t              run_steps;
    ADDR_TYPE             last_pc;
    fdbg_ComputerEvent    event_queue[MAX_EVENTS_STATUS];
    uint8_t               event_count;
    bool                  running;
    fdbg_Status           run_status;
#ifndef MICROCONTROLLER
    int                   fd;
    char                  port[256];
#endif
} FdbgServer;

void fdbg_server_init(FdbgServer* server, uint16_t machine_id, FdbgServerIOCallbacks cb);
void fdbg_server_close(FdbgServer* server);

void fdbg_server_next(FdbgServer* server, FdbgServerEvents* events);

bool fdbg_server_terminal_print(FdbgServer* server, const char* text);
bool fdbg_server_push_event(FdbgServer* server, fdbg_ComputerEvent* event);

void fdbg_debug(FdbgServer* server, const char* fmt, ...);

#ifndef MICROCONTROLLER

bool        fdbg_server_init_pc(FdbgServer* server, uint16_t machine_id, uint32_t baud);
const char* fdbg_server_serial_port(FdbgServer* server);
void        fdbg_die_if_parent_dies();

#endif

#endif //LIBFDBG_SERVER_H_

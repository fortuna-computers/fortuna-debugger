#ifndef LIBFDBG_SERVER_H_
#define LIBFDBG_SERVER_H_

#include <stdint.h>
#include <stddef.h>

#include "../common/common.h"

#include "protobuf/to-debugger.nanopb.h"
#include "protobuf/to-computer.nanopb.h"

// runs either in the emulator or the computer

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
    ADDR_TYPE           (*step)(FdbgServer* server, bool full);
    bool                (*write_memory)(FdbgServer* server, ADDR_TYPE pos, uint8_t* data, uint8_t sz, ADDR_TYPE* first_failed);
    void                (*read_memory)(FdbgServer* server, ADDR_TYPE pos, uint8_t sz, uint8_t* out_data);
    void                (*run_forever)(FdbgServer* server);
} FdbgServerEvents;

FdbgServer* fdbg_server_init(uint16_t machine_id, FdbgServerIOCallbacks cb);
void        fdbg_server_free(FdbgServer* server);

void        fdbg_server_next(FdbgServer* server, FdbgServerEvents* events);

#ifndef MICROCONTROLLER

FdbgServer* fdbg_server_init_pc(uint16_t machine_id, uint32_t baud);
const char* fdbg_server_serial_port(FdbgServer* server);
void        fdbg_die_if_parent_dies();

#endif

#endif //LIBFDBG_SERVER_H_

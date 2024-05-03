#ifndef LIBFDBG_SERVER_H_
#define LIBFDBG_SERVER_H_

#include <stdint.h>
#include <stddef.h>

#include "../common/common.h"

// runs either in the emulator or the computer

#define SERIAL_ERROR   0xfffe
#define SERIAL_NO_DATA 0xffff

typedef struct FdbgServer FdbgServer;

typedef struct FdbgServerIOCallbacks {
    uint16_t (*read_byte_async)(FdbgServer* server);
    void     (*write_byte)(FdbgServer* server, uint8_t byte);
} FdbgServerIOCallbacks;

FdbgServer* fdbg_server_init(FdbgServerIOCallbacks cb);
void        fdbg_server_free(FdbgServer* server);


#ifndef MICROCONTROLLER

FdbgServer* fdbg_server_init_pc(uint32_t baud);
const char* fdbg_server_serial_port(FdbgServer* server);

#endif

#endif //LIBFDBG_SERVER_H_

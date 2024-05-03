#ifndef LIBFDBG_SERVER_H_
#define LIBFDBG_SERVER_H_

#include <stdint.h>
#include <stddef.h>

#include "../common/common.h"

typedef struct FdbgServer FdbgServer;

typedef enum DebuggingLevel { DL_NORMAL, DL_DEBUG, DL_TRACE } DebuggingLevel;

FdbgServer* fdbg_server_init(uint32_t baud);
void        fdbg_server_free(FdbgServer* server);

void        fdbg_server_set_debugging_level(FdbgServer* server, DebuggingLevel d);

const char* fdbg_server_serial_port(FdbgServer* server);

#endif //LIBFDBG_SERVER_H_

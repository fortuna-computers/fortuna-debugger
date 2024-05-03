#ifndef LIBFDBG_CLIENT_H_
#define LIBFDBG_CLIENT_H_

#include <stdint.h>
#include <stddef.h>

#include "../common/common.h"

// used by the debugger or test scripts to communicate with the computer / emulator

typedef enum DebuggingLevel { DL_NORMAL, DL_DEBUG, DL_TRACE } DebuggingLevel;

typedef struct FdbgClient FdbgClient;

FdbgClient* fdbg_client_init_pc(const char* serial_port, uint32_t baud);
void        fdbg_client_free(FdbgClient* client);

void        fdbg_client_set_debugging_level(FdbgClient* client, DebuggingLevel d);

int         fdbg_client_auto_detect_port(const char* vendor_id, const char* product_id, char* result, size_t result_sz);

#endif //LIBFDBG_CLIENT_H_

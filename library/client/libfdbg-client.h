#ifndef LIBFDBG_CLIENT_H_
#define LIBFDBG_CLIENT_H_

#include <stdint.h>
#include <stddef.h>

typedef struct FdbgClient FdbgClient;

FdbgClient* fdbg_client_init(const char* serial_port, uint32_t baud);
void        fdbg_client_free(FdbgClient* client);

int         fdbg_auto_detect_port(const char* vendor_id, const char* product_id, char* result, size_t result_sz);

#endif //LIBFDBG_CLIENT_H_

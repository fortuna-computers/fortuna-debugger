#include "libfdbg-client.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../common/terminal.h"

typedef struct FdbgClient {
    int fd;
    DebuggingLevel debugging_level;
} FdbgClient;

FdbgClient* fdbg_client_init_pc(const char* serial_port, uint32_t baud)
{
    int fd = open(serial_port, O_RDWR);
    if (fd < 0)
        return NULL;

    configure_terminal_settings(fd, baud);

    FdbgClient* client = calloc(1, sizeof(FdbgClient));
    client->fd = fd;
    client->debugging_level = DL_NORMAL;
    return client;
}

void fdbg_client_free(FdbgClient* client)
{
    close(client->fd);
    free(client);
}

void fdbg_client_set_debugging_level(FdbgClient* client, DebuggingLevel d)
{
    client->debugging_level = d;
}

int fdbg_client_auto_detect_port(const char* vendor_id, const char* product_id, char* result, size_t result_sz)
{
    char port[512];

    char cmd[512];
    snprintf(cmd, sizeof cmd, "./findserial.py %s %s", vendor_id, product_id);
    FILE* fp = popen(cmd, "r");
    if (!fp)
        return -1;   // script not found

    fgets(port, sizeof(port), fp);
    if (pclose(fp) != 0) {
        errno = EIO;    // autodetect unsuccessful
        return -1;
    }

    snprintf(result, result_sz, "%s", port);
    return 0;
}

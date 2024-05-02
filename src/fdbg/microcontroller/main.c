#include "../../microcontroller.h"

int main()
{
    microcontroller_init();

    for (;;) {
        if (uart_ready()) {
            // TODO
        }
    }
}
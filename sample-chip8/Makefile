CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I. -I../src/library/server -I../src/library -I../src/contrib/nanopb

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

all: chip8-emulator

chip8-emulator: emulator.o ../src/library/libfdbg-server.a
	$(CC) -o $@ $^ ${LDFLAGS}

clean:
	rm -f emulator.o chip8-emulator

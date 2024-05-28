CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I. -I../src/library/server -I../src/library -I../src/contrib/nanopb -I../src/library/common -I../src/library/protobuf

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

all: s-emulator

s-emulator: emulator.o
	$(CC) -o $@ $^ ../src/library/libfdbg-server.a ${LDFLAGS}

clean:
	rm -f emulator.o s-emulator

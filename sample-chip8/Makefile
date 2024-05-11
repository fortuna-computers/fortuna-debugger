CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I. -Ilua -MMD -fPIC

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

SRC=emulator.o machine.o compiler.o

all: simple-chip8.so

simple-chip8.so: ${SRC}
	$(CC) -shared -o $@ $^ ${LDFLAGS}

clean:
	$(MAKE) -C lua clean
	rm -f ${SRC} simple-chip8.so *.d

-include $(${SRC}:.o=.d)

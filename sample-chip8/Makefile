CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I. -I../src/contrib/lua -MMD -fPIC `pkg-config --cflags lua`

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

SRC=emulator.o machine.o compiler.o

all: simple-chip8.so

simple-chip8.so: ${SRC}
	$(CC) -shared -undefined suppress -o $@ $^ ${LDFLAGS}

clean:
	rm -f ${SRC} simple-chip8.so *.d

-include $(${SRC}:.o=.d)

# flags

CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I. -I../src/contrib/lua -MMD -fPIC $(shell pkg-config --cflags lua)

ifdef RELEASE
	CPPFLAGS += -O3
	LUAC_FLAGS = -s
else
	CPPFLAGS += -O0 -ggdb
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -undefined dynamic_lookup
endif

# sources

SRC = emulator.o machine.o compiler.o

# rules

all: simple-chip8.so

compiler.o: compiler_lua.h    # remove this line to not build a lua compiler file

compiler_lua.h: compiler.lua
	-luac ${LUAC_FLAGS} -o compiler_lua.out $<
	-xxd -i compiler_lua.out > $@
	rm compiler_lua.out

simple-chip8.so: ${SRC}
	$(CC) -shared -o $@ $^ ${LDFLAGS}

clean:
	rm -f ${SRC} $(SRC:.o=.d) simple-chip8.so compiler_lua.h

-include $(${SRC}:.o=.d)

# flags

CFLAGS = --std=gnu17
CPPFLAGS = -Wall -Wextra -I. -I../src/contrib/lua -MMD -fPIC $(shell pkg-config --cflags lua)

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS += -undefined dynamic_lookup
endif

# sources

SRC=emulator.o machine.o compiler.o

# rules

all: simple-chip8.so

simple-chip8.so: ${SRC}
	$(CC) -shared -o $@ $^ ${LDFLAGS}

clean:
	rm -f ${SRC} $(SRC:.o=.d) simple-chip8.so

-include $(${SRC}:.o=.d)

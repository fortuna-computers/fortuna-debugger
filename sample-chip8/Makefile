CFLAGS=--std=gnu17
CPPFLAGS = -Wall -Wextra -I. -MMD -fPIC

SRC=emulator.o machine.o compiler.o

all: simple-chip8.so

simple-chip8.so: ${SRC}
	$(CXX) $^ -shared -o $@ ${LDFLAGS}
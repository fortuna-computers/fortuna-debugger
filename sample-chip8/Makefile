CFLAGS=--std=gnu17
CPPFLAGS = -Wall -Wextra -I. -Ilua -MMD -fPIC

SRC=emulator.o machine.o compiler.o

all: simple-chip8.so

lua/liblua.a:
	$(MAKE) -C lua MYLDFLAGS=

simple-chip8.so: lua/liblua.a ${SRC}
	$(CXX) $^ -shared -o $@ ${LDFLAGS}

clean:
	$(MAKE) -C lua clean
	rm -f ${SRC} simple-chip8.so

-include $(${SRC}:.o=.d)
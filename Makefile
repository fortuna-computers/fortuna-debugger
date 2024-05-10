LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so
TARGETS = f-debugger ${LIBRARY}

all: ${TARGETS}

${LIBRARY}: library-compile

f-debugger: ${LIBRARY} f-debugger-compile simple-chip8-compile

library-compile:
	$(MAKE) -C src/library
	-cp $(addprefix src/library/,${LIBRARY}) .

f-debugger-compile:
	$(MAKE) -C src/debugger
	-cp src/debugger/f-debugger .

simple-chip8-compile:
	$(MAKE) -C sample-chip8
	-cp sample-chip8/simple-chip8.so .

test-library:
	$(MAKE) -C src/library test-library
	./src/library/test-library

test-microcontroller:
	$(MAKE) -C src/library test-microcontroller
	./src/library/test-microcontroller

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	rm -f f-debugger ${LIBRARY}

.PHONY = f-debugger-compile library-compile
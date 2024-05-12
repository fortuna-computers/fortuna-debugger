ifeq ($(OS),Windows_NT)
	$(warning Windows not supported - use it at your own risk)
endif

LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so
TARGETS = f-debugger simple-chip8.so findserial.py ${LIBRARY}

all: ${TARGETS}

${LIBRARY}: library-compile

f-debugger: ${LIBRARY} f-debugger-compile findserial.py

simple-chip8.so: simple-chip8-compile

library-compile:
	$(MAKE) -C src/library
	-cp $(addprefix src/library/,${LIBRARY}) .

f-debugger-compile: ${LIBRARY}
	$(MAKE) -C src/debugger
	-cp src/debugger/f-debugger .

simple-chip8-compile:
	$(MAKE) -C sample-chip8
	-cp sample-chip8/simple-chip8.so .

test-library:
	$(MAKE) -C src/library test-library

test-microcontroller: findserial.py
	$(MAKE) -C src/library test-microcontroller

findserial.py:
	cp src/scripts/findserial.py .

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	$(MAKE) -C sample-chip8 clean
	rm -f ${TARGETS}

.PHONY = f-debugger-compile library-compile
ifeq ($(OS),Windows_NT)
	$(warning Windows not supported - use it at your own risk)
endif

LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so findserial.py
TARGETS = f-debugger simple-chip8.so ${LIBRARY}

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
	./src/library/test-library

test-microcontroller: findserial.py
	$(MAKE) -C src/library test-microcontroller
	./src/library/test-microcontroller

findserial.py:
	cp src/scripts/findserial.py .

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	rm -f ${TARGETS}

.PHONY = f-debugger-compile library-compile
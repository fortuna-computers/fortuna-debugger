ifeq ($(OS),Windows_NT)
	$(warning Windows not supported - use it at your own risk)
endif

LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so fdbg_client.so
TARGETS = f-debugger s-emulator findserial.py fdbg_client.so

all: ${TARGETS}

${LIBRARY}: library-compile

f-debugger: ${LIBRARY} f-debugger-compile findserial.py

s-emulator: s-emulator-compile

libfdbg-server.a: library-compile

library-compile:
	$(MAKE) -C src/library
	-cp $(addprefix src/library/,${LIBRARY}) .

f-debugger-compile: ${LIBRARY}
	$(MAKE) -C src/debugger
	-cp src/debugger/f-debugger .

s-emulator-compile: libfdbg-server.a
	$(MAKE) -C sample-emulator
	-cp sample-emulator/s-emulator .

test-library:
	$(MAKE) -C src/library test-library

test-microcontroller: findserial.py
	$(MAKE) -C src/library test-microcontroller

findserial.py:
	cp src/scripts/findserial.py .

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	$(MAKE) -C sample-emulator clean
	rm -f ${TARGETS} ${LIBRARY}

.PHONY = f-debugger-compile library-compile
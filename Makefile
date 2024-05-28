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

install: all
	install -d /usr/local/bin
	install -s f-debugger /usr/local/bin/
	install findserial.py /usr/local/bin/
	install -d /usr/local/lib /usr/local/lib/pkgconfig
	install libfdbg-client.so libfdbg-client.a libfdbg-server.a /usr/local/lib/
	install -d /usr/local/lib/lua5.4
	install fdbg_client.so /usr/local/lib/lua5.4/
	install -d /usr/local/include/fdbg
	install src/library/server/libfdbg-server.h src/library/client/libfdbg-client.hh /usr/local/include
	cp src/library/common/common.h src/library/protobuf/*.h /usr/local/include/fdbg
	cp src/contrib/nanopb/*.h /usr/local/include/fdbg
	install src/scripts/libfdbg-server.pc /usr/local/lib/pkgconfig

uninstall:
	rm -f /usr/local/bin/f-debugger
	rm -f /usr/local/bin/findserial.py
	rm -f /usr/local/lib/libfdbg-client.so
	rm -f /usr/local/lib/libfdbg-client.a
	rm -f /usr/local/lib/libfdbg-server.a
	rm -f /usr/local/lib/lua5.4/fdbg_client.so
	rm -rf /usr/local/include/libfdbg*.h /usr/local/include/fdbg
	rm -f /usr/local/lib/pkgconfig/libfdbg-server.pc

.PHONY = f-debugger-compile library-compile
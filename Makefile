LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so
TARGETS = f-debugger ${LIBRARY}

all: ${TARGETS}

${LIBRARY}: library-compile

f-debugger: ${LIBRARY} f-debugger-compile

library-compile:
	$(MAKE) -C src/library
	-cp $(addprefix src/library/,${LIBRARY}) .

f-debugger-compile:
	$(MAKE) -C src/debugger
	-cp src/debugger/f-debugger .

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	rm -f f-debugger ${LIBRARY}

.PHONY = f-debugger-compile library-compile
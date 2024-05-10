LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so
TARGETS = f-debugger ${LIBRARY}

all: ${TARGETS}

f-debugger:
	$(MAKE) -C src/debugger
	-cp src/debugger/f-debugger .

${LIBRARY}:
	$(MAKE) -C src/library
	-cp $(addprefix src/library/,${LIBRARY}) .

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	rm -f f-debugger ${LIBRARY}
LIBRARY = libfdbg-server.a libfdbg-client.a libfdbg-client.so

all:
	$(MAKE) -C src/library
	$(MAKE) -C src/debugger
	-cp $(addprefix src/library/,${LIBRARY}) .
	-cp src/debugger/f-debugger .

clean:
	$(MAKE) -C src/library clean
	$(MAKE) -C src/debugger clean
	rm -f f-debugger ${LIBRARY}
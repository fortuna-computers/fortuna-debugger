#
# dependencies
#

ifeq ($(OS),Windows_NT)
	$(warning Windows not supported - use it at your own risk)
endif

#
# directories
#

LIBRARY := src/library
DEBUGGER := src/debugger

#
# sources
#

EXE = ${LIBRARY}/server/libfdbg-server.a \
      ${LIBRARY}/client/libfdbg-client.a \
 	  ${DEBUGGER}/f-debugger

#
# rules
#

all: build

${LIBRARY}/client/libfdbg-client.so: FORCE
	$(MAKE) -C ${LIBRARY}/client libfdbg-client.so

${LIBRARY}/client/lua/fdbg_client.so: FORCE
	$(MAKE) -C ${LIBRARY}/client/lua

src/contrib/libtmt/libtmt.c:  # git submodules
	git submodule update --init --recursive

${EXE}: src/contrib/libtmt/libtmt.c FORCE
	$(MAKE) -C ${DEBUGGER}

build: ${EXE} ${LIBRARY}/client/libfdbg-client.so ${LIBRARY}/client/lua/fdbg_client.so
	mkdir -p build
	cp $^ build/
.PHONY: build

#
# special rules
#

clean:
	$(MAKE) -C ${LIBRARY}/server clean
	$(MAKE) -C ${LIBRARY}/client clean
	$(MAKE) -C ${DEBUGGER} clean
	rm -rf build
.PHONY: clean

FORCE:

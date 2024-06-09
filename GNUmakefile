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
SCRIPTS := src/scripts

#
# sources
#

BIN = ${LIBRARY}/server/libfdbg-server.a \
      ${LIBRARY}/client/libfdbg-client.a \
      ${LIBRARY}/client/libfdbg-client.so \
      ${LIBRARY}/client/lua/fdbg_client.so \
 	  ${DEBUGGER}/f-debugger \
 	  ${SCRIPTS}/findserial.py

#
# rules
#

export CPPFLAGS

all: CPPFLAGS=-O0 -ggdb
all: build

release: CPPFLAGS=-Ofast
release: build

build:
	$(MAKE) -C ${LIBRARY}/client libfdbg-client.so
	$(MAKE) -C ${LIBRARY}/client/lua
	$(MAKE) -C ${DEBUGGER}
	cp ${BIN} .

src/contrib/libtmt/tmt.c:  # git submodules
	git submodule update --init --recursive

#
# special rules
#

clean:
	$(MAKE) -C ${LIBRARY}/server clean
	$(MAKE) -C ${LIBRARY}/client clean
	$(MAKE) -C ${DEBUGGER} clean
	rm -f *.a *.so f-debugger
.PHONY: clean

FORCE: src/contrib/libtmt/tmt.c

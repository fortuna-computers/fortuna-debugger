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

BIN = ${LIBRARY}/server/libfdbg-server.a \
      ${LIBRARY}/client/libfdbg-client.a \
      ${LIBRARY}/client/libfdbg-client.so \
      ${LIBRARY}/client/lua/fdbg_client.so \
 	  ${DEBUGGER}/f-debugger

#
# rules
#

all:
	$(MAKE) -C ${LIBRARY}/client libfdbg-client.so
	$(MAKE) -C ${LIBRARY}/client/lua
	$(MAKE) -C ${DEBUGGER}
	cp ${BIN} .

src/contrib/libtmt/tmt.c:  # git submodules
	git submodule update --init --recursive

build: ${EXE} ${LIBRARY}/client/libfdbg-client.so ${LIBRARY}/client/lua/fdbg_client.so
	echo "==> BUILD"
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
	rm -f *.a *.so f-debugger
.PHONY: clean

FORCE: src/contrib/libtmt/tmt.c

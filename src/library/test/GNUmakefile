# directories

PROTOBUF := ../protobuf
CONTRIB := ../../contrib
NANOPB := ${CONTRIB}/nanopb

# flags

CPPFLAGS += -Wall -Wextra $(shell pkg-config --cflags lua protobuf) -I../server -I../client -I../common -I${NANOPB}
CXXFLAGS += -std=c++20
LDFLAGS += $(shell pkg-config --libs lua protobuf)

# rules

all: test-library

test: test-library
	./test-library

../client/libfdbg-client.a: FORCE
	$(MAKE) -C ../client libfdbg-client.a

../server/libfdbg-server.a: FORCE
	$(MAKE) -C ../server libfdbg-server.a

test-library: test-library.o ../client/libfdbg-client.a ../server/libfdbg-server.a
	$(CXX) -o $@ $^ ${LDFLAGS}

# special rules

clean:
	rm -f *.o test-library
.PHONY: clean

FORCE:
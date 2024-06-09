# directories

CONTRIB := ../../../contrib
PROTOBUF := ../protobuf
NANOPB := ${CONTRIB}/nanopb

# flags

CPPFLAGS += -Wall -Wextra -fPIC $(shell pkg-config --cflags lua protobuf) -I../../common -I${NANOPB}
CXXFLAGS += --std=c++20
LDFLAGS += $(shell pkg-config --libs protobuf lua)

# rules

all: fdbg_client.so

../libfdbg-client.a: FORCE
	$(MAKE) -C .. libfdbg-client.a

lua-client.o: ../libfdbg-client.a

fdbg_client.so: lua-client.o ../libfdbg-client.a
	$(CXX) $^ -shared -o $@ ${LDFLAGS}

# special rules

clean:
	rm -f *.o *.so
.PHONY: clean

FORCE:

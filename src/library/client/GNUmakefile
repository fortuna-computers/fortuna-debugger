# dependencies

K := $(if $(shell which protoc),,$(error "protoc not found in PATH - please install protobuf")))
K := $(if $(shell pkg-config --modversion protobuf),,$(error "protobuf pkg-config file not found - please install protobuf"))
K := $(if $(shell pkg-config --modversion lua),,$(error "lua pkg-config file not found - please install Lua"))

# directories

CONTRIB := ../../contrib
PROTOBUF := ../protobuf

# programs

PROTOC ?= protoc

# flags

CPPFLAGS += -MMD -Wall -Wextra -fPIC -I../common -I${CONTRIB}/nanopb $(shell pkg-config --cflags lua protobuf)
CXXFLAGS += --std=c++20
LDFLAGS += $(shell pkg-config --libs lua protobuf)

# sources

GENERATED_PROTO_FILES := to-computer.pb.cc to-debugger.pb.cc to-computer.pb.h to-debugger.pb.h
GENERATED_PROTO_FILES_SRC := $(filter %.cc,${GENERATED_PROTO_FILES})

LIBFDBG_CLIENT_SRC = libfdbg-client.o machine.o

# rules

all: libfdbg-client.a libfdbg-client.so lua/fdbg_client.so

${GENERATED_PROTO_FILES}: ${PROTOBUF}/to-computer.proto ${PROTOBUF}/to-debugger.proto
	protoc --cpp_out=. --proto_path=${PROTOBUF} $^

libfdbg-client.o: ${GENERATED_PROTO_FILES}

libfdbg-client.a: $(GENERATED_PROTO_FILES_SRC:.cc=.o) ${LIBFDBG_CLIENT_SRC}
	ar -rc $@ $^

libfdbg-client.so: $(GENERATED_PROTO_FILES_SRC:.cc=.o) ${LIBFDBG_CLIENT_SRC}
	$(CXX) $^ -shared -o $@ ${LDFLAGS}

lua/fdbg_client.so:
	$(MAKE) -j$(shell nproc) -C lua

# special rules

clean:
	rm -f ${GENERATED_PROTO_FILES} *.o *.a *.d *.so
	$(MAKE) -C lua clean
.PHONY: clean

-include $(LIBFDBG_CLIENT_SRC:.o=.d)
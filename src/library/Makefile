BASE=../..

include ${BASE}/defs.mk

# 
# check for dependencies
#

K := $(if $(shell which python3),,$(error "python3 not found in PATH - please install it")))
K := $(if $(shell which protoc),,$(error "protoc not found in PATH - please install protobuf")))
K := $(if $(shell pkg-config --modversion protobuf),,$(error "protobuf pkg-config file not found - please install protobuf"))
K := $(if $(shell pkg-config --modversion lua),,$(error "lua pkg-config file not found - please install Lua"))

#
# flags
#

CPPFLAGS += $(shell pkg-config --cflags lua) -DADDR_TYPE=uint64_t
LUA_LIBS = $(shell pkg-config --libs lua)

#
# targets
#

TARGET = libfdbg-server.a libfdbg-client.a libfdbg-client.so fdbg_client.so test-library test-microcontroller

.PHONY: all
all: $(TARGET)

#
# libfdbg-server
#

GENERATED_NANOPB_FILES := protobuf/to-computer.nanopb.c protobuf/to-debugger.nanopb.c

${GENERATED_NANOPB_FILES}: protobuf/to-computer.proto protobuf/to-debugger.proto
	${NANOPB_GEN} -e .nanopb $^

server/libfdbg-server.o: ${GENERATED_NANOPB_FILES}

LIBFDBG_SERVER_SRC = \
        common/terminal.o \
        server/libfdbg-server.o

NANOPB_SRC = \
        $(GENERATED_NANOPB_FILES:.c=.o) \
        ${NANOPB}/pb_common.o \
        ${NANOPB}/pb_encode.o \
        ${NANOPB}/pb_decode.o \

libfdbg-server.a: ${NANOPB_SRC} ${LIBFDBG_SERVER_SRC} 
	ar -rc $@ $^

-include $(LIBFDBG_SERVER_SRC:.o=.d)

#
# libfdbg-client
#

GENERATED_PROTO_FILES := protobuf/to-computer.pb.cc protobuf/to-debugger.pb.cc

${GENERATED_PROTO_FILES}: protobuf/to-computer.proto protobuf/to-debugger.proto
	protoc --cpp_out=. $^

LIBFDBG_CLIENT_SRC = \
		common/terminal.o \
		client/libfdbg-client.o \
		client/machine.o

client/libfdbg-client.o: ${GENERATED_PROTO_FILES}

libfdbg-client.a: $(GENERATED_PROTO_FILES:.cc=.o) ${LIBFDBG_CLIENT_SRC} 
	ar -rc $@ $^

libfdbg-client.so: $(GENERATED_PROTO_FILES:.cc=.o) ${LIBFDBG_CLIENT_SRC}
	$(CXX) $^ -shared -o $@ $(PROTOBUF_LDFLAGS) ${LUA_LIBS}

fdbg_client.so: client/lua/lua-client.o libfdbg-client.a
	$(CXX) $^ -shared -o $@ $(PROTOBUF_LDFLAGS) ${LUA_LIBS}

-include $(LIBFDBG_CLIENT_SRC:.o=.d)

#
# tests
#

test/test-library.o: libfdbg-client.a libfdbg-server.a   # fix ordering issue

test-library: test/test-library.o libfdbg-client.a libfdbg-server.a
	$(CXX) -o $@ $^ $(PROTOBUF_LDFLAGS) ${LUA_LIBS}

test/test-microcontroller.o: libfdbg-client.a

test-microcontroller: test/test-microcontroller.o libfdbg-client.a
	$(CXX) -o $@ $^ $(PROTOBUF_LDFLAGS) ${LUA_LIBS}

#
# clean
#

.PHONY: clean
clean:
	find . -type f -name '*.[od]' -delete
	rm -f ${TARGET} ${NANOPB_SRC} ${GENERATED_NANOPB_FILES} ${NANOPB}/*.d $(GENERATED_NANOPB_FILES:.c=.h) ${GENERATED_PROTO_FILES} $(GENERATED_PROTO_FILES:.cc=.h)

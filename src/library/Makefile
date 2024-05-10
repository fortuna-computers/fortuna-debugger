BASE=../..
DEPENDENCIES = python3 protoc

include ${BASE}/mk/defs.mk

# 
# check for dependencies
#

TARGET = libfdbg-server.a libfdbg-client.a libfdbg-client.so test-library test-microcontroller

.PHONY: all
all: $(TARGET)

#
# libfdbg-server
#

GENERATED_NANOPB_FILES := protobuf/to-computer.nanopb.c protobuf/to-debugger.nanopb.c

${GENERATED_NANOPB_FILES}: protobuf/to-computer.proto protobuf/to-debugger.proto
	${NANOPB_GEN} -e .nanopb $^

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
		client/libfdbg-client.o

libfdbg-client.a: $(GENERATED_PROTO_FILES:.cc=.o) ${LIBFDBG_CLIENT_SRC} 
	ar -rc $@ $^

libfdbg-client.so: $(GENERATED_PROTO_FILES:.cc=.o) ${LIBFDBG_CLIENT_SRC}
	$(CXX) $^ -shared -o $@ $(PROTOBUF_LDFLAGS)

-include $(LIBFDBG_CLIENT_SRC:.o=.d)

#
# tests
#

test-library: test/test-library.o libfdbg-client.a libfdbg-server.a
	$(CXX) -o $@ $^ $(PROTOBUF_LDFLAGS)

test-microcontroller: test/test-microcontroller.o libfdbg-client.a
	$(CXX) -o $@ $^ $(PROTOBUF_LDFLAGS)

#
# clean
#

.PHONY: clean
clean:
	rm -f ${TARGET} *.o *.d **/*.o **/*.d ${GENERATED_NANOPB_FILES} $(GENERATED_NANOPB_FILES:.c=.h) ${GENERATED_PROTO_FILES} $(GENERATED_PROTO_FILES:.cc=.h) ${NANOPB}/*.d

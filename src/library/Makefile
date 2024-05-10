BASE=../..
DEPENDENCIES = python3 protoc

include ${BASE}/mk/defs.mk

# 
# check for dependencies
#

.PHONY: all
all: libfdbg-server.a libfdbg-client.a libfdbg-client.so

#
# libfdbg-server
#

GENERATED_NANOPB_FILES := protobuf/to-computer.nanopb.c protobuf/to-debugger.nanopb.c

${GENERATED_NANOPB_FILES}: protobuf/to-computer.proto protobuf/to-debugger.proto
	${NANOPB_GEN} -e .nanopb $^

server/libfdbg-server.o: ${GENERATED_NANOPB_FILES}  # generate protos first

LIBFDBG_SERVER_SRC = \
        common/terminal.o \
        server/libfdbg-server.o

NANOPB_SRC = \
        ${NANOPB}/pb_common.o \
        ${NANOPB}/pb_encode.o \
        ${NANOPB}/pb_decode.o \
        $(GENERATED_NANOPB_FILES:.c=.o)

libfdbg-server.a: ${LIBFDBG_SERVER_SRC} ${NANOPB_SRC}
	ar -rc $@ $^

-include $(LIBFDBG_SERVER_SRC:.o=.d)

#
# libfdbg-client
#

GENERATED_PROTO_FILES := protobuf/to-computer.pb.cc protobuf/to-debugger.pb.cc

${GENERATED_PROTO_FILES}: protobuf/to-computer.proto protobuf/to-debugger.proto
	protoc --cpp_out=. $^

client/libfdbg-client.o: ${GENERATED_PROTO_FILES}

LIBFDBG_CLIENT_SRC = \
		common/terminal.o \
		client/libfdbg-client.o

libfdbg-client.a: ${LIBFDBG_CLIENT_SRC} ${GENERATED_PROTO_FILES}
	ar -rc $@ $^

libfdbg-client.so: ${LIBFDBG_CLIENT_SRC} ${GENERATED_PROTO_FILES}
	gcc $< -shared -o $@ $(PROTOBUF_LDFLAGS)

-include $(LIBFDBG_CLIENT_SRC:.o=.d)


#
# clean
#

.PHONY: clean
clean:
	rm -f *.a *.o *.d **/*.o **/*.d ${GENERATED_NANOPB_FILES} $(GENERATED_NANOPB_FILES:.c=.h) ${GENERATED_PROTO_FILES $(GENERATED_PROTO_FILES:.cc=.h) ${NANOPB}/*.d

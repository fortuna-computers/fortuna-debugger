BASE=../..

include ${BASE}/mk/defs.mk

.PHONY: all
all: libfdbg-server.a

#
# nanopb proto files
#

GENERATED_PROTO_FILES := protobuf/to-computer.pb.c protobuf/to-debugger.pb.c

${GENERATED_PROTO_FILES}: protobuf/to-computer.proto protobuf/to-debugger.proto
	${NANOPB_GEN} $^

#
# libfdbg-server
#

server/libfdbg-server.o: ${GENERATED_PROTO_FILES}  # generate protos first

LIBFDBG_SERVER_SRC = \
        common/terminal.o \
        server/libfdbg-server.o

NANOPB_SRC = \
        ${NANOPB}/pb_common.o \
        ${NANOPB}/pb_encode.o \
        ${NANOPB}/pb_decode.o \
        $(GENERATED_PROTO_FILES:.c=.o)

libfdbg-server.a: ${LIBFDBG_SERVER_SRC} ${NANOPB_SRC}
	ar -rc $@ $^

-include $(LIBFDBG_SERVER_SRC:.o=.d)

#
# clean
#

.PHONY: clean
clean:
	rm -f *.a *.o *.d **/*.o **/*.d ${GENERATED_PROTO_FILES} $(GENERATED_PROTO_FILES:.c=.h) ${NANOPB}/*.d

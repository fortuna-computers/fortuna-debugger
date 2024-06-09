# dependencies

K := $(if $(shell which python3),,$(error "python3 not found in PATH - please install it")))
K := $(if $(shell pkg-config --modversion lua),,$(error "lua pkg-config file not found - please install Lua"))

# directories

PROTOBUF := ../protobuf
CONTRIB := ../../contrib
NANOPB := ${CONTRIB}/nanopb

# programs

NANOPB_GEN := python3 ${NANOPB}/generator/nanopb_generator.py

# flags

CPPFLAGS += -MMD -Wall -Wextra -I${CONTRIB}/nanopb -I../common -fPIC $(shell pkg-config --cflags lua) -DADDR_TYPE=uint64_t

# sources

GENERATED_NANOPB_FILES := to-computer.pb.c to-debugger.pb.c to-computer.pb.h to-debugger.pb.h

NANOPB_SRC = \
        $(GENERATED_NANOPB_FILES:.c=.o) \
        ${NANOPB}/pb_common.o \
        ${NANOPB}/pb_encode.o \
        ${NANOPB}/pb_decode.o \

# rules

all: libfdbg-server.a

${GENERATED_NANOPB_FILES}: ${PROTOBUF}/to-computer.proto ${PROTOBUF}/to-debugger.proto
	${NANOPB_GEN} -D . --proto-path=${PROTOBUF} $^

libfdbg-server.o: ${GENERATED_NANOPB_FILES}

libfdbg-server.a: ${NANOPB_SRC} libfdbg-server.o
	ar -rc $@ $^

# special rules

clean:
	rm -f ${GENERATED_NANOPB_FILES} *.o *.a *.d
.PHONY: clean

-include $(LIBFDBG_SERVER_SRC:.o=.d)
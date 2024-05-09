BASE=../..

include ${BASE}/mk/defs.mk

.PHONY: all
all: ${OUTDIR}/libfdbg-server.a

#
# nanopb proto files
#

GENERATED_PROTO_FILES := ${OUTDIR}/${PROTO}/to-computer.pb.c ${OUTDIR}/${PROTO}/to-debugger.pb.c

${GENERATED_PROTO_FILES}: ${SRCDIR}/${PROTO}/to-computer.proto ${SRCDIR}/${PROTO}/to-debugger.proto
	@mkdir -p ${OUTDIR}/${PROTO}
	${NANOPB_GEN} -I ${SRCDIR} -D ${OUTDIR} $^

#
# libfdbg-server
#

${OUTDIR}/library/server/libfdbg-server.o: ${GENERATED_PROTO_FILES} # generate protos first

LIBFDBG_SERVER_SRC = \
        ${OUTDIR}/library/common/terminal.o \
        ${OUTDIR}/library/server/libfdbg-server.o

NANOPB_SRC = \
        ${OUTDIR}/contrib/nanopb/pb_common.o \
        ${OUTDIR}/contrib/nanopb/pb_encode.o \
        ${OUTDIR}/contrib/nanopb/pb_decode.o \
        $(GENERATED_PROTO_FILES:.c=.o)

${OUTDIR}/libfdbg-server.a: ${LIBFDBG_SERVER_SRC} ${NANOPB_SRC}
	ar -rc $@ $^

# include $(LIBFDBG_SERVER_SRC:%.o=%.d)

#
# clean
#

.PHONY: clean
clean:
	rm -rf ${OUTDIR}
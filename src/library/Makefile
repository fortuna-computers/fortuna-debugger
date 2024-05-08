BASE=../..

include ${BASE}/mk/defs.mk

all: ${OUTDIR}/libfdbg-server.so

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

${OUTDIR}/library/server/libfdbg-server.o: ${GENERATED_PROTO_FILES}

LIBFDBG_SERVER_SRC = \
        ${OUTDIR}/library/common/terminal.o \
        ${OUTDIR}/library/server/libfdbg-server.o \
        ${OUTDIR}/${PROTO}/to-computer.pb.o \
        ${OUTDIR}/${PROTO}/to-debugger.pb.o

${OUTDIR}/libfdbg-server.so: ${LIBFDBG_SERVER_SRC}
	${CC} -shared -o $@ $^ ${LDFLAGS}

#
# clean
#

clean:
	rm -rf ${OUTDIR}/library ${OUTDIR}/libfdbg-server.so
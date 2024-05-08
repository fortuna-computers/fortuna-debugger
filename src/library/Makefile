BASE=../..

include ${BASE}/mk/defs.mk

all: ${OUTDIR}/libfdbg-server.so

#
# nanopb proto files
#

${OUTDIR_NANOPB}/to-computer.pb.c: ${PROTO}/to-computer.proto
	@mkdir -p ${OUTDIR_NANOPB}
	${NANOPB_GEN} -I ${PROTO} -D ${OUTDIR_NANOPB} $^

${OUTDIR_NANOPB}/to-debugger.pb.c: ${PROTO}/to-debugger.proto
	@mkdir -p ${OUTDIR_NANOPB}
	${NANOPB_GEN} -I ${PROTO} -D ${OUTDIR_NANOPB} $^

#
# libfdbg-server
#

${BASE}/src/library/server/libfdbg-server.o: ${OUTDIR}/nanopb/to-computer.pb.c ${OUTDIR}/nanopb/to-debugger.pb.c

NANOPB_OBJ = \
		${NANOPB}/pb_common.o \
		${NANOPB}/pb_decode.o \
		${NANOPB}/pb_encode.o

LIBFDBG_SERVER_SRC = \
        ${BASESRC}/library/server/libfdbg-server.o \
        ${BASESRC}/library/common/terminal.o \
		${OUTDIR_NANOPB}/to-computer.pb.o \
		${OUTDIR_NANOPB}/to-debugger.pb.o \
		${NANOPB_OBJ}

${OUTDIR}/libfdbg-server.so: ${LIBFDBG_SERVER_SRC}
	${CC} -shared -o $@ $(foreach obj,$^,${OUTDIR}/obj/$(notdir ${obj})) ${LDFLAGS}

#
# clean
#

clean:
	rm -rf ${OUTDIR}/nanopb ${OUTDIR}/obj ${OUTDIR}/libfdbg-server.so
ifndef BASE
  $(error BASE is not set)
endif

#
# directories
#

OUTDIR=${BASE}/build

BASESRC=${BASE}/src
PROTO=${BASESRC}/library/protobuf
OUTDIR_NANOPB=${OUTDIR}/nanopb

CONTRIB=${BASE}/src/contrib
NANOPB=${CONTRIB}/nanopb

#
# special commands
#

NANOPB_GEN=${PYTHON} ${NANOPB}/generator/nanopb_generator.py

PYTHON=python3

#
# compilation flags
#

INCLUDES = -I. -I${OUTDIR} -I${NANOPB} -I${OUTDIR_NANOPB}
CPPFLAGS = -Wall -Wextra ${INCLUDES}

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

#
# automatic rules
#

%.o: %.c
	@mkdir -p ${OUTDIR}/obj
	${CC} ${CFLAGS} ${CPPFLAGS} -o ${OUTDIR}/obj/$(notdir $@) -c $<
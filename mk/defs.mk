ifndef BASE
  $(error BASE is not set)
endif

#
# directories
#

OUTDIR=${BASE}/build
SRCDIR=${BASE}/src
VPATH := ${SRCDIR}

PROTO=library/protobuf

CONTRIB=${SRCDIR}/contrib
NANOPB=${CONTRIB}/nanopb

#
# special commands
#

PYTHON=python3
NANOPB_GEN=${PYTHON} ${NANOPB}/generator/nanopb_generator.py

#
# compilation flags
#

INCLUDES = -I. -I${OUTDIR} -I${NANOPB}
CPPFLAGS = -Wall -Wextra ${INCLUDES}

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

#
# automatic rules
#

${OUTDIR}/${PROTO}/%.o: ${OUTDIR}/${PROTO}/%.c
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ -c $^

%.o: $(subst .o,.c,$(subst ${OUTDIR},${SRCDIR},$@))
	mkdir -p $(*D)
	${CC} ${CFLAGS} ${CPPFLAGS} -o $@ -c $(subst .o,.c,$(subst ${OUTDIR},${SRCDIR},$@))

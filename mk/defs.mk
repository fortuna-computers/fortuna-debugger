ifndef BASE
  $(error BASE is not set)
endif

#
# directories
#


CONTRIB=${BASE}/src/contrib
NANOPB=${CONTRIB}/nanopb

#
# special commands
#

PYTHON=python3
NANOPB_GEN=${PYTHON} ${NANOPB}/generator/nanopb_generator.py

#
# compilation flags
#

INCLUDES = -I. -I${NANOPB}
CPPFLAGS = -MMD -Wall -Wextra ${INCLUDES}

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

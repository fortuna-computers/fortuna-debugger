ifndef BASE
  $(error BASE is not set)
endif

#
# check for dependencies
#

K := $(foreach exec,${DEPENDENCIES},\
	 $(if $(shell which $(exec)),,$(error "No $(exec) found in PATH")))

#
# directories
#

CONTRIB=${BASE}/src/contrib
NANOPB=${CONTRIB}/nanopb

#
# special commands
#

NANOPB_GEN=python3 ${NANOPB}/generator/nanopb_generator.py

#
# compilation flags
#

INCLUDES = -I. -I${NANOPB}
CFLAGS = --std=gnu17
CPPFLAGS = -MMD -Wall -Wextra -fPIC ${INCLUDES} \
		   `pkg-config --cflags protobuf`
CXXFLAGS = --std=c++20

PROTOBUF_LDFLAGS = `pkg-config --libs protobuf-lite`

ifdef RELEASE
	CPPFLAGS += -O3
else
	CPPFLAGS += -O0 -ggdb
endif

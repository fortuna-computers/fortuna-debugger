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

NANOPB_GEN=python3 ${NANOPB}/generator/nanopb_generator.py

#
# compilation flags
#

INCLUDES = -I. -I${NANOPB} -I${BASE}/src/library/common -I${BASE}/src/library/protobuf
CFLAGS = --std=gnu17
CPPFLAGS = -MMD -Wall -Wextra -fPIC ${INCLUDES} $(shell pkg-config --cflags protobuf)
CXXFLAGS = --std=c++20

PROTOBUF_LDFLAGS = `pkg-config --libs protobuf`

ifdef DEBUG
	CPPFLAGS += -O0 -ggdb -DDEBUG=1 -DDISABLE_ERROR_HANDLING=1
else
	CPPFLAGS += -O3 -DNDEBUG=1
endif

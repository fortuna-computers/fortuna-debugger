OUTDIR = build
GENDIR = generated

SOEXT = so
ifeq ($(OS),Windows_NT)
	SOEXT = dll
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		SOEXT = dylib
	endif
endif

# protobuf files

${GENDIR}:
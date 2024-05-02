#!/bin/sh

../../fdbg/contrib/nanopb/generator/nanopb_generator.py -I ../protobuf to-computer.proto
../../fdbg/contrib/nanopb/generator/nanopb_generator.py -I ../protobuf to-debugger.proto

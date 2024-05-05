#!/bin/sh

../contrib/nanopb/generator/nanopb_generator.py -I ../library/protobuf to-computer.proto
../contrib/nanopb/generator/nanopb_generator.py -I ../library/protobuf to-debugger.proto

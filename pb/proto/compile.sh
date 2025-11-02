#!/bin/sh
rm -f ../*.h ../*.cc
protoc --cpp_out=../ referee_command.proto common.proto packet.proto command.proto replacement.proto vssref_common.proto vssref_command.proto vssref_placement.proto



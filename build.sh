#!/bin/bash

buildpath=./x64/Debug/ACEngine
buildtarget=debug64
clear
#make clean -s
make $buildtarget && \
chmod +x $buildpath && \
echo '----------------' && \
echo '-build complete-' && \
echo '----------------' && \
echo '' && \
$buildpath


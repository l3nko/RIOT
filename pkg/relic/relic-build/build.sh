#!/bin/bash
ls |grep -v build.sh |xargs rm -r
COMP="-g -m32" LINK="-m32" cmake -DARCH=X86 -DWORD=32 -DDEBUG=on -DSEED=ZERO -DWORD=32 -DWITH="ALL" -DCMAKE_INSTALL_PREFIX:PATH=.  ../relic-src/
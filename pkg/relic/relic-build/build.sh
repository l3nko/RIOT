#!/bin/bash
ls |grep -v build.sh |xargs rm -r
COMP="-g -m32" LINK="-m32" cmake -DARCH=X86 -DSEED=ZERO -DWORD=32 -DWITH="ALL" -DDCMAKE_INSTALL_PREFIX=/Users/magenta/Desktop/l3nko-RIOT/pkg/relic/relic-build/ ../relic-src/
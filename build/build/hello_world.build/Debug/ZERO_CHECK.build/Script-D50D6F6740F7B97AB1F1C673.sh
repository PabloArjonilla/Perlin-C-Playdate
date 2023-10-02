#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  make -f /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  make -f /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/CMakeScripts/ReRunCMake.make
fi


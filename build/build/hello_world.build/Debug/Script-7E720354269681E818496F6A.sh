#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  /opt/homebrew/Cellar/cmake/3.27.6/bin/cmake -E copy /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Debug/libhello_world.dylib /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/Source/pdex.dylib
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  /Users/pablomac/Developer/PlaydateSDK/bin/pdc -sdkpath /Users/pablomac/Developer/PlaydateSDK /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/Source /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/hello_world.pdx
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  /opt/homebrew/Cellar/cmake/3.27.6/bin/cmake -E cmake_symlink_library /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Debug/libhello_world.dylib /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Debug/libhello_world.dylib /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Debug/libhello_world.dylib
fi
if test "$CONFIGURATION" = "Release"; then :
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  /opt/homebrew/Cellar/cmake/3.27.6/bin/cmake -E copy /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Release/libhello_world.dylib /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/Source/pdex.dylib
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  /Users/pablomac/Developer/PlaydateSDK/bin/pdc -sdkpath /Users/pablomac/Developer/PlaydateSDK /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/Source /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/hello_world.pdx
  cd "/Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello World/build"
  /opt/homebrew/Cellar/cmake/3.27.6/bin/cmake -E cmake_symlink_library /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Release/libhello_world.dylib /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Release/libhello_world.dylib /Users/pablomac/Documents/Desarrollo/gamedev/playdate/examples/Hello\ World/build/Release/libhello_world.dylib
fi


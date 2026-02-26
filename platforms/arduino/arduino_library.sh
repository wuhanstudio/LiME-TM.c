#!/bin/sh

# 1. Remove all .c and .h files in src folder
rm -f src/*.c
rm -f src/*.h

# 2. Copy .c and .h files from tsetlin, utils, random into src
cp -f ../../tsetlin/*.c src/ 2>/dev/null
cp -f ../../tsetlin/*.h src/ 2>/dev/null

cp -f ../../utils/*.c src/ 2>/dev/null
cp -f ../../utils/*.h src/ 2>/dev/null

cp -f ../../random/*.c src/ 2>/dev/null
cp -f ../../random/*.h src/ 2>/dev/null

echo "Done."
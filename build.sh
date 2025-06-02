#!/bin/bash
echo "Building MUX compiler..."

SRC="src"
OUT="muxc"

# Compile all sources (C++ and C)
g++ "$SRC/main.cpp" \
    "$SRC/parser.cpp" \
    "$SRC/utils.cpp" \
    "$SRC/lexer.cpp" \
    "$SRC/codegen.c" -o "$OUT"

if [ $? -ne 0 ]; then
    echo "Build failed."
else
    echo "MUX compiler built successfully: $OUT"
fi

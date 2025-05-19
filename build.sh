#!/bin/bash
echo "Building MUX compiler..."

# Clean up
rm -f *.o # Remove object files
rm -f muxc # Remove the executable (no extension needed in Linux/macOS)

# Compile C++ object files
g++ -c src/main.cpp -o main.o
g++ -c src/lexer.cpp -o lexer.o
g++ -c src/parser.cpp -o parser.o
g++ -c src/utils.cpp -o utils.o

# Compile C file
gcc -c src/codegen.c -o codegen.o

# Link to output
g++ main.o lexer.o parser.o utils.o codegen.o -o muxc

if [ -f "muxc" ]; then # Use -f to check for file existence
    echo "MUX compiler built successfully."
else
    echo "Build failed."
fi

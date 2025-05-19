@echo off
echo Building MUX compiler...

:: Clean up
del *.o >nul 2>&1
del muxc.exe >nul 2>&1

:: Compile C++ object files
g++ -c src\main.cpp -o main.o
g++ -c src\lexer.cpp -o lexer.o
g++ -c src\parser.cpp -o parser.o
g++ -c src\utils.cpp -o utils.o

:: Compile C file
gcc -c src\codegen.c -o codegen.o

:: Link to output
g++ main.o lexer.o parser.o utils.o codegen.o -o muxc.exe

if exist muxc.exe (
    echo MUX compiler built successfully.
) else (
    echo Build failed.
)

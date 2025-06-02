@echo off
echo Building muxc compiler...

:: Clean up any existing files
if exist main.o del main.o
if exist lexer.o del lexer.o
if exist parser.o del parser.o
if exist utils.o del utils.o
if exist codegen.o del codegen.o
if exist muxc.exe del muxc.exe

:: Compile all cpp files
echo Compiling source files...
g++ -c src/main.cpp -o main.o
if errorlevel 1 goto error

g++ -c src/lexer.cpp -o lexer.o
if errorlevel 1 goto error

g++ -c src/parser.cpp -o parser.o
if errorlevel 1 goto error

g++ -c src/utils.cpp -o utils.o
if errorlevel 1 goto error

g++ -c src/codegen.cpp -o codegen.o
if errorlevel 1 goto error

:: Link all object files
echo Linking...
g++ main.o lexer.o parser.o utils.o codegen.o -o muxc.exe
if errorlevel 1 goto error

:: Clean up object files
echo Cleaning up...
del *.o

echo Build complete.
if exist muxc.exe (
    echo Successfully created muxc.exe
    exit /b 0
) else (
    echo Failed to create muxc.exe
    exit /b 1
)

:error
echo Build failed.
exit /b 1

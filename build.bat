@echo off
echo Building MUX compiler...

setlocal
set SRC=src
set OUT=muxc.exe

REM Compile without generating .o files
g++ "%SRC%\main.cpp" "%SRC%\parser.cpp" "%SRC%\lexer.cpp" "%SRC%\utils.cpp" "%SRC%\codegen.c" -o %OUT%

if %errorlevel% neq 0 (
    echo  Build failed.
) else (
    echo  MUX compiler built successfully: %OUT%
)
endlocal

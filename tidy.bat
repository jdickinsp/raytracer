@set INCLUDES=-I./include -I../shared_libraries/stb
@set SOURCES=./src/*.c

clang-tidy %SOURCES% -- %INCLUDES%

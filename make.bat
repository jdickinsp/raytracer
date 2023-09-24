@REM Build for Clang compiler

@set INCLUDES=-I./include -I../shared_libraries/stb
@set SOURCES=./src/raytracer.c ./src/camera.c ./src/examples.c ./src/image.c ./src/loaders.c ./src/material.c ./src/matrix44.c ./src/raycast.c ./src/scene.c ./src/texture.c ./src/utils.c ./src/vectors.c 
@set LIBS=-openmp

clang-cl %INCLUDES% %SOURCES% -O2 -o ./bin/raytracer.exe %LIBS%
.\bin\raytracer.exe

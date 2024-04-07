@REM Build for Clang compiler

@set INCLUDES=-I./include -I./extern/stb
@set SOURCES=./src/render.c ./src/binary_tree.c ./src/bvh.c ./src/camera.c ./src/collections.c ./src/examples.c ./src/image.c ./src/loaders.c ./src/material.c ./src/matrix44.c ./src/object_mesh.c ./src/path_trace.c ./src/ray_trace.c  ./src/raycast.c ./src/scene.c ./src/texture.c ./src/utils.c ./src/vectors.c 
@set LIBS=-openmp
@set XARGS=/fp:fast
@REM @set DEBUG_ARGS=/Zi /EHsc

@REM mkdir bin
@REM mkdir outputs

clang-cl %INCLUDES% %SOURCES% -O2 -o ./bin/raytracer.exe %LIBS% %XARGS%
@REM .\bin\raytracer.exe

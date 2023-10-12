@REM Build for Clang compiler

@set INCLUDES=-I./include -I../shared_libraries/stb
@set SOURCES=./src/raytracer.c ./src/binary_tree.c ./src/bvh.c ./src/camera.c ./src/collections.c ./src/examples.c ./src/image.c ./src/loaders.c ./src/material.c ./src/matrix44.c ./src/raycast.c ./src/scene.c ./src/texture.c ./src/utils.c ./src/vectors.c 
@set LIBS=-openmp
@set XARGS=/Zi /EHsc

clang-cl %INCLUDES% %SOURCES% -fuse-ld=lld -o ./bin/raytracer-debug.exe %LIBS% %XARGS%

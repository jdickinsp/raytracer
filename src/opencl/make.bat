@set INCLUDES=-IC:\dev\vcpkg\packages\opencl_x64-windows\include
@set SOURCE=./main.c
@set LIBS=..\..\..\vcpkg\packages\opencl_x64-windows\lib\OpenCL.lib
clang-cl %INCLUDES% %SOURCE% -o ./bin/main.exe %LIBS%

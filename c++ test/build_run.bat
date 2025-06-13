@echo off

set PATH=%PATH%;C:\w64devkit\bin
xcopy resources bin\resources\ /y /s
g++ main.cpp glad.c -o bin/cube_test.exe -DWINDOWS -I include/ -L lib/ -lSDL3 -lopengl32
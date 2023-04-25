#!/bin/bash

pushd bin
gcc -W -mavx -g -I/home/rafael/Work/libs ~/Work/libs/XLibs/examples/x11-ubuntu/main.c -o x11_opengl -lm -lX11 -lEGL
popd
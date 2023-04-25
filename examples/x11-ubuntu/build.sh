#!/bin/bash

pushd bin
gcc -W -mavx -g -I/home/rafael/Work/libs ~/Desktop/x11/main.c -o x11_opengl -lm -lX11 -lEGL
popd
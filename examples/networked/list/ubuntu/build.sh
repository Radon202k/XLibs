#!/bin/bash

pushd bin
gcc -W -mavx -g -I/home/rafael/Work/libs ~/Work/libs/XLibs/examples/networked/list/ubuntu/main.c -o x11_opengl -lm -lX11 -lEGL

gcc -W -mavx -g -I/home/rafael/Work/libs ~/Work/libs/XLibs/examples/networked/list/ubuntu/headless_server.c -o list_server -lm
popd
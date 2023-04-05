@echo off

set warn=-wd4189 -wd4100 -wd4201 -wd4042 -wd4115

IF NOT EXIST bin mkdir bin

cls

pushd bin
cl -Z7 -FC -W4 %warn% -nologo ../main.c
popd
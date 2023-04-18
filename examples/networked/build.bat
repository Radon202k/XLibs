@echo off

set warn=-W4 -wd4189 -wd4100 -wd4201 -wd4042 -wd4115

set comp=-WL -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 -GS- -Gs9999999

IF NOT EXIST bin mkdir bin

cls

pushd bin
cl -MTd %warn% %comp% ../main.c
popd
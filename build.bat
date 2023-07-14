@echo off

IF NOT EXIST "bin/" MKDIR "bin/"

SET CC=gcc
SET CFLAGS=-Wall -Werror
SET LIB=-lm

SET SRCS=main.c image.c .\filters\blur.c .\filters\grayscale.c .\filters\sharpen.c .\filters\median.c .\filters\bilateral.c .\filters\box.c .\filters\laplacian.c .\filters\sobel.c

%CC% %CFLAGS% %SRCS% -o bin/cv.exe %LIB%

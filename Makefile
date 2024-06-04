CLANG := /home/eric2011/src/llvm-project/build-clang/bin/clang++
CC := gcc
CPP := g++
DBGFLAGS := -g

all: hello.o rtlib.o
	$(CPP) -o hello hello.o rtlib.o
hello.o: hello.cpp
	$(CLANG) -c -fpass-plugin=`echo build/passes/hw4.so` hello.cpp
rtlib.o: rtlib.c
	$(CC) -c rtlib.c
clean:
	rm -f hello hello.o rtlib.o
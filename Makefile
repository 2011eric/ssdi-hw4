CLANG := /home/eric2011/src/llvm-project/build-clang/bin/clang++
DBGFLAGS := -g

hello: test/hello.cpp
	$(CLANG) -o hello -fpass-plugin=`echo build/passes/hw4.so` test/hello.cpp
clean:
	rm -f hello hello.o
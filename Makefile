CPPFLAGS += -std=c++20 -W -Wall -g -I include

.PHONY: default

default: bin/c_compiler

all : src/parser.tab.cpp src/parser.tab.hpp src/lexer.yy.cpp bin/c_compiler compiler

src/parser.tab.cpp src/parser.tab.hpp : src/parser.y
	@echo "Compiling Parser"
	bison -v -d src/parser.y -o src/parser.tab.cpp


src/lexer.yy.cpp : src/lexer.flex src/parser.tab.hpp
	@echo "Compiling Flex"
	flex -o src/lexer.yy.cpp  src/lexer.flex

bin/c_compiler :  src/cli.cpp src/compiler.o src/parser.tab.o src/lexer.yy.o src/parser.tab.o
	@echo "Compiling compiler"
	mkdir -p bin
	g++ $(CPPFLAGS) -o bin/c_compiler $^

compiler : bin/c_compiler

clean :
	rm -rf bin/*
	rm src/*.tab.cpp
	rm src/*.tab.hpp
	rm src/*.output
	rm src/*.yy.cpp
	rm src/*.o

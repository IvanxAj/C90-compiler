CPPFLAGS += -W -Wall -g

# This avoids error: ‘fileno’ was not declared in this scope
CPPFLAGS += -std=c++0x

# Avoid warnings about yyunput not used
CPPFLAGS += -Wno-unused-function

all : token_test

lexer.yy.cpp : lexer.flex
	flex -o lexer.yy.cpp  lexer.flex

token_test : lexer.yy.o token_test.o
	g++ $(CPPFLAGS) -o token_test  lexer.yy.o token_test.o

clean :
	-rm -f token_test token_test.exe token_test.o lexer.yy.o
	-rm -f *.yy.cpp
	-rm -f test/out/*

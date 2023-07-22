#ifndef token_test.hpp
#define token_test.hpp

#include <string>

enum TokenType {
    None = 0,
    T_INT = 1,
    T_RETURN = 2,
    T_LBRACKET = 3,
    T_RBRACKET = 4,
    T_LCBRACKET = 5,
    T_RCBRACKET = 6,
    T_SEMICOLON = 7,
    IDENTIFIER = 8,
    INT_LITERALS = 9
};

union TokenValue{
    double numberValue;
    std::string *wordValue;
};


// This is a global variable used to move the
// attribute value from the lexer back to the
// main program.
// By convention it is called yylval, as that is
// the name that will be automatically generated
// by Bison (see next lab).
extern TokenValue yylval;

// This is the lexer function defined by flex. Each
// time it is called, a token type value will be
// returned.
extern int yylex();

#endif

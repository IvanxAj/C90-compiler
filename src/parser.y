%code requires{
  #include "ast.hpp"
  #include <cassert>

  extern const Expression *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

// Represents the value associated with any kind of
// AST node.
%union{
  const Expression *expr;
  double number;
  std::string *string;
}

%token T_TIMES T_DIVIDE T_PLUS T_MINUS T_EXPONENT
%token T_LBRACKET T_RBRACKET
%token T_LOG T_EXP T_SQRT
%token T_NUMBER T_VARIABLE

%type <expr> EXPR TERM UNARY FACTOR
%type <number> T_NUMBER
%type <string> T_VARIABLE T_LOG T_EXP T_SQRT

%start ROOT

%%

ROOT : EXPR { g_root = $1; }

/* TODO-3 : Add support for (x + 6) and (10 - y). You'll need to add production rules, and create an AddOperator or
            SubOperator. */
EXPR : TERM               { $$ = $1; }
     | EXPR T_PLUS TERM   { $$ = new AddOperator( $1, $3 ); }
     | EXPR T_MINUS TERM  { $$ = new SubOperator( $1, $3 ); }
     ;

/* TODO-4 : Add support (x * 6) and (z / 11). */
TERM : UNARY          { $$ = $1; }
      | TERM T_TIMES UNARY { $$ = new MulOperator( $1, $3 ); }
      | TERM T_DIVIDE UNARY { $$ = new DivOperator( $1, $3 ); }
      ;

/*  TODO-5 : Add support for (- 5) and (- x). You'll need to add production rules for the unary minus operator and create a NegOperator. */
UNARY : FACTOR        { $$ = $1; }
      | T_MINUS UNARY { $$ = new NegOperator( $2 ); }
      | FACTOR T_EXPONENT UNARY { $$ = new ExpOperator( $1, $3 ); }
      ;

FACTOR : T_NUMBER     { $$ = new Number( $1 ); }
       | T_VARIABLE   { $$ = new Variable( *$1 ); }
       | T_LBRACKET EXPR T_RBRACKET { $$ = $2; }
       | T_LOG T_LBRACKET EXPR T_RBRACKET { $$ = new LogFunction($3); }
       | T_EXP T_LBRACKET EXPR T_RBRACKET { $$ = new ExpFunction($3); }
       | T_SQRT T_LBRACKET EXPR T_RBRACKET { $$ = new SqrtFunction($3); }
       ;

%%

const Expression *g_root; // Definition of variable (to match declaration earlier)

const Expression *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}

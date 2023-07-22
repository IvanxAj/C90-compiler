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

%token T_TIMES T_DIVIDE T_PLUS T_MINUS
%token T_LBRACKET T_RBRACKET
%token T_LCBRACKET T_RCBRACKET
%token T_SEMICOLON T_ASSIGNMENT
%token T_NUMBER T_VARIABLE
%token T_INT
%token T_RETURN
%token IDENTIFIER INT_LITERALS

%type <expr>   declarator direct_declarator declaration_specifier type_specifier primary_expression compound_statement
%type <expr>   statement expression_statement jump_statement expression assignment_expression unary_expression postfix_expression
%type <number> INT_LITERALS
%type <string> IDENTFIER

%start ROOT

%%


ROOT : function_definition { g_root = $1; }

function_definition
    : declaration_specifier declarator compound_statement { }
    ;

// name of stuff (variable / function etc)
declarator
    : direct_declarator { $$ = $1; }
    ;

direct_declarator
    : IDENTIFIER { new Declarator() }
    | T_LBRACKET declarator T_RBRACKET    { $$ = $2; }
    | direct_declarator T_LBRACKET T_RBRACKET { $$ = new FunctionDeclarator($1); }
    ;


// type of stuff
declaration_specifier
    : type_specifier { $$ = $1; }
    ;

type_specifier
    : T_INT { $$ = new PrimitiveType{_int}; }
    ;

compound_statement
    : T_LCBRACKET T_RCBRACKET                   { scope stuff }
    | T_LCBRACKET statement_list T_RCBRACKET    { scope stuff }
    ;

statement_list
    : statement                     { initialise statement "list" }
    | statement_list statement      { add to statement list }

statement
    : compound_statement        { $$ = $1; }
    | expression_statement      { $$ = $1; }
    | jump_statement            { $$ = $1; }
    ;

expression_statement
    : T_SEMICOLON               { ; }
    | expression T_SEMICOLON    { $$ = $1; }
    ;

jump_statement
    : T_RETURN T_SEMICOLON              { new Return(); }
    | T_RETURN expression T_SEMICOLON   { new Return($2); }
    ;

expression
    : assignment_expression     { $$ = $1; }
    ;

assignment_expression
    : unary_expression                                      { $$ = $1; }
    | unary_expression T_ASSIGNMENT assignment_expression   { }
    ;

unary_expression
    : postfix_expression    { $$ = $1; }
    ;

postfix_expression
    : primary_expression                       { $$ = $1; }
    | postfix_expression T_LBRACKET T_RBRACKET { $$ = new FunctionCall($1); }
    ;

primary_expression
	: IDENTIFIER          { $$ = new Identifier(*$1); }
    | INT_LITERALS        { $$ = new Integer($1); }
//	| CONSTANT
//	| STRING_LITERAL
//	| '(' expression ')'
	;

%%



const Expression *g_root; // Definition of variable (to match declaration earlier)

const Expression *parseAST()
{
  g_root=0;
  yyparse();
  return g_root;
}

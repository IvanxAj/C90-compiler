%code requires{
  #include "ast.hpp"
  #include <cassert>
  #include <iostream>

  extern const Node *g_root; // A way of getting the AST out

  //! This is to fix problems when generating C++
  // We are declaring the functions provided by Flex, so
  // that Bison generated code can call them.
  int yylex(void);
  void yyerror(const char *);
}

 // Represents the value associated with any kind of
 // AST node.
%union{
    double number;
    std::string *string;

    const Node *node;
    BaseExpression *base_expression;
    Declarator *declarator_node;

    // BaseList *base_list;
    List_Ptr node_list;
}

 /* ----------------------------------------------------------          Tokens           -------------------------------------------------------------- */

 // Arithmetic operators
%token T_TIMES T_DIVIDE T_PLUS T_MINUS
 // Character Operators
%token T_LBRACKET T_RBRACKET T_LCBRACKET T_RCBRACKET T_SEMICOLON T_ASSIGNMENT
 // Types operators
%token T_INT
 // Control flow operators
%token T_RETURN
 // Stuff
%token IDENTIFIER INT_LITERALS

 /* ----------------------------------------------------------          Types           -------------------------------------------------------------- */

%type <node>   external_declaration function_definition compound_statement declaration_specifier type_specifier
%type <node>   declaration statement expression_statement jump_statement

%type <node_list> translation_unit statement_list declaration_list
%type <declarator_node>  declarator direct_declarator init_declarator
%type <base_expression> expression assignment_expression unary_expression postfix_expression primary_expression initializer
%type <number> INT_LITERALS
%type <string> IDENTIFIER

%start ROOT

%%


ROOT : translation_unit { g_root = new ProgramRoot($1); }

translation_unit
	: external_declaration                  { $$ = createList($1); }
	| translation_unit external_declaration { $$ = appendList($1, $2); }
	;

external_declaration
    : function_definition  { $$ = $1; }
    | declaration          { $$ = $1; }
    ;

declaration_list
	: declaration                       { $$ = createList($1); }
	| declaration_list declaration      { $$ = appendList($1, $2); }
	;

declaration
    : declaration_specifier T_SEMICOLON
	| declaration_specifier init_declarator T_SEMICOLON     { $$ = new Declaration($1, $2); }
	;


function_definition
    : declaration_specifier declarator compound_statement { $$ = new FunctionDefinition($2, $3); }
    ;

 /*  type of stuff */
declaration_specifier
    : type_specifier { $$ = $1; }
    ;

 /*$$ = new PrimitiveType(INT);*/
type_specifier
    : T_INT { ; }
    ;

init_declarator
	: declarator                                { $$ = new Init_Declarator($1); }
	| declarator T_ASSIGNMENT initializer       { $$ = new Init_Declarator($1, $3); }
	;

 /* name of stuff (variable / function etc) */
declarator
    : direct_declarator { $$ = $1; }
    /* pointer direct_declarator - handle pointers here */
    ;

direct_declarator
    : IDENTIFIER                                                { $$ = new Declarator(*$1); delete $1; }
    | direct_declarator T_LBRACKET T_RBRACKET                   { $$ = $1; }
    /* | direct_declarator T_LBRACKET parameter_list T_RBRACKET    { $$ = new FuncDeclarator } */
    ;

initializer
	: assignment_expression { $$ = $1;}
	/* | '{' initializer_list '}' { $$ = new ArrayInitializer($2);}  */
	;

compound_statement
    : T_LCBRACKET statement_list T_RCBRACKET                    { $$ = new CompoundStatement($2); }
    | T_LCBRACKET declaration_list T_RCBRACKET
    | T_LCBRACKET declaration_list statement_list T_RCBRACKET
    /* : T_LCBRACKET T_RCBRACKET                   { scope stuff } */
    ;

 /* Assuming only one statement */
statement_list
    : statement                     { $$ = createList($1); }
    | statement_list statement      { $$ = appendList($1, $2); }
    ;

statement
    : jump_statement            { $$ = $1; }
    /* : compound_statement        { $$ = $1; }
    | expression_statement      { $$ = $1; }
    */
    ;

expression_statement
    : T_SEMICOLON               { ; }
    | expression T_SEMICOLON    { $$ = $1; }
    ;

jump_statement
    : T_RETURN expression T_SEMICOLON   { $$ = new Return($2); }
    ;

expression
    : assignment_expression     { $$ = $1; }
    ;

assignment_expression
    : unary_expression                                      { $$ = $1; }
    /* | unary_expression T_ASSIGNMENT assignment_expression */
    ;

unary_expression
    : postfix_expression    { $$ = $1; }
    ;

postfix_expression
    : primary_expression                            { $$ = $1; }
    | postfix_expression T_LBRACKET T_RBRACKET      { $$ = new FunctionCall($1); }
    ;

primary_expression
    : IDENTIFIER          { $$ = new Identifier(*$1); delete $1; }
    | INT_LITERALS        { $$ = new Integer($1); }
	/*   | CONSTANT
	| STRING_LITERAL
	| '(' expression ')' */
	;

%%



const Node *g_root; // Definition of variable (to match declaration earlier)

const Node *parseAST()
{
  g_root=NULL;
  yyparse();
  return g_root;
}

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
    BaseDeclaration *base_declaration;
    BaseStatement *base_statement;

    Specifier type;

    // BaseList *base_list;
    List_Ptr node_list;
}

 /* ----------------------------------------------------------          Tokens           -------------------------------------------------------------- */

 // Arithmetic operators
%token T_LEFT_OP T_RIGHT_OP T_INC_OP T_DEC_OP T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_RIGHT_ASSIGN T_LEFT_ASSIGN
 // Comparison Operators
%token T_AND_OP T_OR_OP T_LE_OP T_GE_OP T_EQ_OP T_NE_OP T_AND_ASSIGN T_XOR_ASSIGN T_OR_ASSIGN
 // Types operators
%token T_INT T_CHAR T_SIZEOF T_UNSIGNED T_VOID T_FLOAT T_DOUBLE
 // Control flow operators
%token T_RETURN T_BREAK T_CONTINUE
// Conditional statements
%token T_IF T_ELSE T_WHILE T_FOR T_DO T_SWITCH T_CASE T_DEFAULT
 // Stuff
%token IDENTIFIER INT_LITERALS FLOAT_LITERALS

 /* ----------------------------------------------------------          Types           -------------------------------------------------------------- */

%type <node>   external_declaration function_definition
%type <node>   declaration

%type <node_list> translation_unit statement_list declaration_list parameter_list argument_expression_list

%type <base_declaration>  declarator direct_declarator init_declarator parameter_declaration
%type <base_statement> statement expression_statement jump_statement compound_statement selection_statement iteration_statement
%type <base_statement> labeled_statement
%type <base_expression> expression assignment_expression unary_expression postfix_expression primary_expression initializer
%type <base_expression> additive_expression multiplicative_expression shift_expression

%type <base_expression> conditional_expression logical_or_expression logical_and_expression inclusive_or_expression
%type <base_expression> exclusive_or_expression relational_expression and_expression equality_expression
%type <base_expression> constant_expression

%type <number> INT_LITERALS FLOAT_LITERALS
%type <string> IDENTIFIER
%type <type> declaration_specifier type_specifier

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
	: declaration_specifier init_declarator ';'     { $$ = new Declaration($1, $2); }
	;


function_definition
    : declaration_specifier declarator compound_statement { $$ = new FunctionDefinition($1, $2, $3); }
    ;

 /*  type of stuff */
declaration_specifier
    : type_specifier { $$ = $1; }
    ;

/* Treat unsigned as int for now */
type_specifier
    : T_INT         { $$ = Specifier::_int; }
    | T_FLOAT       { $$ = Specifier::_float; }
    | T_DOUBLE      { $$ = Specifier::_double; }
    | T_CHAR        { $$ = Specifier::_char; }
    | T_VOID        { $$ = Specifier::_void; }
	| T_UNSIGNED    { $$ = Specifier::_int; }
    ;

init_declarator
	: declarator                       { $$ = new Init_Declarator($1); }
	| declarator '=' initializer       { $$ = new Init_Declarator($1, $3); }
	;

 /* name of stuff (variable / function etc) */
declarator
    : direct_declarator { $$ = $1; }
    /* pointer direct_declarator - handle pointers here */
    ;

// update to use FunctionDeclarator
direct_declarator
    : IDENTIFIER                                    { $$ = new Declarator(*$1); delete $1; }
    | direct_declarator '(' ')'                     { $$ = new FuncDeclarator($1); }
    | direct_declarator '(' parameter_list ')'      { $$ = new FuncDeclarator($1, $3); }
    | direct_declarator '[' ']'                     { std::cerr << "Array decl"; }
    | direct_declarator '[' constant_expression ']' { std::cerr << "Array decl"; }
    ;

parameter_list
	: parameter_declaration                         { $$ = createList($1); }
	| parameter_list ',' parameter_declaration      { $$ = appendList($1, $3); }
    ;

parameter_declaration
	: type_specifier declarator     { $$ = new ParamDeclaration($1, $2); }
	;

initializer
	: assignment_expression { $$ = $1;}
	/* | '{' initializer_list '}' { $$ = new ArrayInitializer($2);}  */
	;

compound_statement
    : '{' statement_list '}'                        { $$ = new CompoundStatement($2, nullptr); }
    | '{' declaration_list '}'                      { $$ = new CompoundStatement(nullptr, $2); }
    | '{' declaration_list statement_list '}'       { $$ = new CompoundStatement($3, $2); }
    | '{' '}'                                       { $$ = new CompoundStatement(); }
    ;

 /* Assuming only one statement */
statement_list
    : statement                     { $$ = createList($1); }
    | statement_list statement      { $$ = appendList($1, $2); }
    ;

statement
    : jump_statement            { $$ = $1; }
    | expression_statement      { $$ = $1; }
    | compound_statement        { $$ = $1; }
    | selection_statement       { $$ = $1; }
	| iteration_statement       { $$ = $1; }
    | labeled_statement         { $$ = $1; }
    ;

expression_statement
    : ';'               {}
    | expression ';'    { $$ = new ExpressionStatement($1); }
    ;

selection_statement
	: T_IF '(' expression ')' statement                     { $$ = new IfElse($3, $5);}
	| T_IF '(' expression ')' statement T_ELSE statement    { $$ = new IfElse($3, $5, $7);}
    | T_SWITCH '(' expression ')' statement                 { $$ = new Switch($3, $5); }
	;

iteration_statement
	: T_WHILE '(' expression ')' statement                                              { $$ = new While($3, $5); }
	| T_FOR '(' expression_statement expression_statement ')' statement                 { $$ = new For($3, $4, $6);}
	| T_FOR '(' expression_statement expression_statement expression ')' statement      { $$ = new For($3, $4, $5, $7);}
	;

labeled_statement
	: T_CASE constant_expression ':' statement      { $$ = new Case($2, $4); }
	| T_DEFAULT ':' statement                       { $$ = new Case($3); }
	;

jump_statement
    : T_RETURN expression ';'       { $$ = new JumpStatement(JumpType::Return, $2); }
	| T_RETURN ';'	                { $$ = new JumpStatement(JumpType::Return);  }
    | T_CONTINUE ';'	            { $$ = new JumpStatement(JumpType::Continue); }
	| T_BREAK ';'	                { $$ = new JumpStatement(JumpType::Break); }
    ;


expression
    : assignment_expression     { $$ = $1; }
    ;

constant_expression
	: conditional_expression { $$ = $1;}
	;

assignment_expression
    : conditional_expression                                    { $$ = $1; }
    | conditional_expression '=' assignment_expression          { $$ = new Assignment($1, $3); }
	| unary_expression T_ADD_ASSIGN assignment_expression       { $$ = new Assignment($1, new Addition($1, $3, false)); }
	| unary_expression T_SUB_ASSIGN assignment_expression       { $$ = new Assignment($1, new Subtraction($1, $3, false)); }
    | unary_expression T_MUL_ASSIGN assignment_expression       { $$ = new Assignment($1, new Multiplication($1, $3, false)); }
	| unary_expression T_DIV_ASSIGN assignment_expression       { $$ = new Assignment($1, new Division($1, $3, false)); }
	| unary_expression T_MOD_ASSIGN assignment_expression       { $$ = new Assignment($1, new Modulus($1, $3, false)); }
	| unary_expression T_LEFT_ASSIGN assignment_expression      { $$ = new Assignment($1, new LeftShift($1, $3, false)); }
	| unary_expression T_RIGHT_ASSIGN assignment_expression     { $$ = new Assignment($1, new RightShift($1, $3, false)); }
	| unary_expression T_AND_ASSIGN assignment_expression       { $$ = new Assignment($1, new BitwiseAnd($1, $3, false)); }
	| unary_expression T_XOR_ASSIGN assignment_expression       { $$ = new Assignment($1, new BitwiseXor($1, $3, false)); }
	| unary_expression T_OR_ASSIGN assignment_expression        { $$ = new Assignment($1, new BitwiseOr($1, $3, false)); }
	;

conditional_expression
	: logical_or_expression { $$ = $1;}
	;

logical_or_expression
	: logical_and_expression { $$ = $1;}
	| logical_or_expression T_OR_OP logical_and_expression  { $$ = new LogicalOr($1, $3);}
	;

logical_and_expression
	: inclusive_or_expression { $$ = $1;}
	| logical_and_expression T_AND_OP inclusive_or_expression { $$ = new LogicalAnd($1, $3); }
	;

inclusive_or_expression
	: exclusive_or_expression { $$ = $1;}
	| inclusive_or_expression '|' exclusive_or_expression   { $$ = new BitwiseOr($1, $3); }
	;

exclusive_or_expression
	: and_expression { $$ = $1;}
	| exclusive_or_expression '^' and_expression    { $$ = new BitwiseXor($1, $3);}
	;

and_expression
	: equality_expression { $$ = $1;}
	| and_expression '&' equality_expression    { $$ = new BitwiseAnd($1, $3);}
	;

equality_expression
	: relational_expression { $$ = $1;}
	| equality_expression T_EQ_OP relational_expression { $$ = new Equal($1, $3);}
	| equality_expression T_NE_OP relational_expression { $$ = new NotEqual($1, $3);}
	;

relational_expression
	: shift_expression                              { $$ = $1;}
	| relational_expression '<' shift_expression    { $$ = new LessThan($1, $3);}
	| relational_expression '>' shift_expression    { $$ = new GreaterThan($1, $3);}
	| relational_expression T_LE_OP shift_expression  { $$ = new LessThanEqual($1, $3); }
	| relational_expression T_GE_OP shift_expression  { $$ = new GreaterThanEqual($1, $3); }
	;

shift_expression
	: additive_expression                               { $$ = $1; }
	| shift_expression T_LEFT_OP additive_expression    { $$ = new LeftShift($1, $3); }
	| shift_expression T_RIGHT_OP additive_expression   { $$ = new RightShift($1, $3); }
	;

additive_expression
	: multiplicative_expression                             { $$ = $1; }
	| additive_expression '+' multiplicative_expression     { $$ = new Addition($1, $3); }
	| additive_expression '-' multiplicative_expression     { $$ = new Subtraction($1, $3); }
	;

multiplicative_expression
	: unary_expression                                  { $$ = $1; }
	| multiplicative_expression '*' unary_expression    { $$ = new Multiplication($1, $3); }
	| multiplicative_expression '/' unary_expression    { $$ = new Division($1, $3); }
	| multiplicative_expression '%' unary_expression    { $$ = new Modulus($1, $3); }
	;

unary_expression
    : postfix_expression                { $$ = $1; }
    | T_INC_OP unary_expression         { $$ = new Increment($2); }
    | T_DEC_OP unary_expression         { $$ = new Decrement($2); }
  	| '+' unary_expression              { $$ = new UnaryOp(UnaryOperator::Plus, $2); }
	| '-' unary_expression              { $$ = new UnaryOp(UnaryOperator::Minus, $2); }
	| '!' unary_expression              { $$ = new UnaryOp(UnaryOperator::LogicalNot, $2); }
	| '~' unary_expression              { $$ = new UnaryOp(UnaryOperator::BitwiseNot, $2); }
	| '*' unary_expression              { $$ = new UnaryOp(UnaryOperator::Deref, $2); }
	| '&' unary_expression              { $$ = new UnaryOp(UnaryOperator::AddressOf, $2); }
    | T_SIZEOF unary_expression         { $$ = new SizeOf($2);}
	| T_SIZEOF '(' type_specifier ')'   { $$ = new SizeOf($3);}
    ;

argument_expression_list
	: assignment_expression                                 { $$ = createList($1); }
	| argument_expression_list ',' assignment_expression    { $$ = appendList($1, $3); }
	;

postfix_expression
    : primary_expression                                        { $$ = $1; }
    | postfix_expression '(' ')'                                { $$ = new FunctionCall($1); }
    | postfix_expression '(' argument_expression_list ')'       { $$ = new FunctionCall($1, $3);}
    | postfix_expression '[' expression ']'                     { std::cerr << "Array index"; }
    | postfix_expression T_INC_OP                               { $$ = new Increment($1);}
	| postfix_expression T_DEC_OP                               { $$ = new Decrement($1);}
    ;

primary_expression
    : IDENTIFIER          { $$ = new Identifier(*$1); delete $1; }
    | INT_LITERALS        { $$ = new Number($1, Specifier::_int); }
    | FLOAT_LITERALS      { $$ = new Number($1, Specifier::_float); }
    | '(' expression ')'  { $$ = $2; }
	;

%%



const Node *g_root;

const Node *parseAST()
{
  g_root=NULL;
  yyparse();
  return g_root;
}

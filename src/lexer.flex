%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"

int current_line = 1;

%}

D			          [0-9]
L               [a-zA-Z_]
H			          [a-fA-F0-9]
FS			        (f|F|l|L)
IS			        (u|U|l|L)*

%%

\n              { ++current_line; }

"int"           { return(T_INT); }
"char"          { return(T_CHAR); }
"void"          { return(T_VOID); }
"unsigned"      { return(T_UNSIGNED); }
"double"        { return(T_DOUBLE); }
"float"         { return(T_FLOAT); }

"return"        { return(T_RETURN); }
"if"			      { return(T_IF); }
"else"			    { return(T_ELSE); }
"switch"			  { return(T_SWITCH); }
"case"			    { return(T_CASE); }
"default"       { return(T_DEFAULT); }
"while"			    { return(T_WHILE); }
"do"            { return(T_DO); }
"continue"      { return(T_CONTINUE); }
"break"         { return(T_BREAK); }
"for"			      { return(T_FOR); }
"struct"			    { return(T_STRUCT); }
"union"			    { return(T_UNION); }
"enum"			    { return(T_ENUM); }
"sizeof"        { return(T_SIZEOF); }

{D}+"."{D}+{FS}         { yylval.number = std::stod(yytext); return FLOAT_LITERALS; }
{D}+"."{D}+             { yylval.number = std::stod(yytext); return DOUBLE_LITERALS; }
{D}+                    { yylval.number = std::stod(yytext); return INT_LITERALS; }
{L}({L}|{D})*           { yylval.string = new std::string(yytext); return IDENTIFIER; }


"&&"			{ return(T_AND_OP); }
"||"			{ return(T_OR_OP); }
"<="			{ return(T_LE_OP); }
">="			{ return(T_GE_OP); }
"=="			{ return(T_EQ_OP); }
"!="			{ return(T_NE_OP); }

">>="			{ return(T_RIGHT_ASSIGN); }
"<<="			{ return(T_LEFT_ASSIGN); }
"+="			{ return(T_ADD_ASSIGN); }
"-="			{ return(T_SUB_ASSIGN); }
"*="			{ return(T_MUL_ASSIGN); }
"/="			{ return(T_DIV_ASSIGN); }
"%="			{ return(T_MOD_ASSIGN); }
"&="			{ return(T_AND_ASSIGN); }
"^="			{ return(T_XOR_ASSIGN); }
"|="			{ return(T_OR_ASSIGN); }

">>"			      { return(T_RIGHT_OP); }
"<<"			      { return(T_LEFT_OP); }
"++"			      { return(T_INC_OP); }
"--"			      { return(T_DEC_OP); }

"*"             { return('*'); }
"+"             { return('+'); }
"/"             { return('/'); }
"-"             { return('-'); }
"%"             { return('%'); }
"="             { return('='); }

"&"		    	{ return('&'); }
"!"		    	{ return('!'); }
"<"		    	{ return('<'); }
">"		    	{ return('>'); }
"?"		    	{ return('?'); }
"^"		    	{ return('^'); }
"|"		    	{ return('|'); }

"("		    	    { return('('); }
")"		    	    { return(')'); }
"{"             { return('{'); }
"}"             { return('}'); }
"["             { return('['); }
"]"             { return(']'); }

";"             { return(';'); }
":"             { return(':'); }
","             { return(','); }
"."		    	    { return('.'); }

[ \t\r]+		{ }

.               { fprintf(stderr, "Invalid token at line %d\n", current_line); exit(1); }
%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error at line %d: %s\n", current_line, s);  // Include the line number
  exit(1);
}

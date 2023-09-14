%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"

int current_line = 1;

%}

D			          [0-9]
L               [a-zA-Z_]

%%

\n              { ++current_line; }

"int"           { return(T_INT); }
"char"          { return(T_CHAR); }
"void"          { return(T_VOID); }
"unsigned"      { return(T_UNSIGNED); }
"return"        { return(T_RETURN); }
"if"			      { return(T_IF); }
"else"			    { return(T_ELSE); }
"switch"			  { return(T_SWITCH); }
"case"			    { return(T_CASE); }
"while"			    { return(T_WHILE); }
"do"            { return(T_DO); }
"continue"      { return(T_CONTINUE); }
"break"         { return(T_BREAK); }
"for"			      { return(T_FOR); }
"sizeof"        { return(T_SIZEOF); }

{L}({L}|{D})*   { yylval.string = new std::string(yytext); return IDENTIFIER; }
{D}+            { yylval.number = atoi(yytext); return INT_LITERALS; }

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

"&&"			{ return(AND_OP); }
"||"			{ return(OR_OP); }
"<="			{ return(LE_OP); }
">="			{ return(GE_OP); }
"=="			{ return(EQ_OP); }
"!="			{ return(NE_OP); }

">>"			      { return(T_RIGHT_OP); }
"<<"			      { return(T_LEFT_OP); }
"++"			      { return(T_INC_OP); }
"--"			      { return(T_DEC_OP); }

"("		    	    { return('('); }
")"		    	    { return(')'); }
"{"             { return('{'); }
"}"             { return('}'); }

";"             { return(';'); }
","             { return(','); }
"."		    	    { return('.'); }

[ \t\r\n]+		{ }

.               { fprintf(stderr, "Invalid token at line %d\n", current_line); exit(1); }
%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error at line %d: %s\n", current_line, s);  // Include the line number
  exit(1);
}

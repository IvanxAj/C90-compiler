%option noyywrap

%{
// Avoid error "error: `fileno' was not declared in this scope"
extern "C" int fileno(FILE *stream);

#include "parser.tab.hpp"

%}

D			          [0-9]
L               [a-zA-Z_]

%%

"int"           { return(T_INT); }
"return"        { return(T_RETURN); }
"if"			      { return(T_IF); }
"else"			    { return(T_ELSE); }
"while"			    { return(T_WHILE); }
"for"			      { return(T_FOR); }
"sizeof"        { return(T_SIZEOF)}

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

.               { fprintf(stderr, "Invalid token\n"); exit(1); }
%%

void yyerror (char const *s)
{
  fprintf (stderr, "Parse error : %s\n", s);
  exit(1);
}

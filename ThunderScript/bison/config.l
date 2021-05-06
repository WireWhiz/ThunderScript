
%{
#include "config.tab.hh"
#include "../src/ThunderScriptCompiler.h"
#undef  YY_DECL
#define YY_DECL int ts::tsScanner::yylex( ts::tsParser::semantic_type * const lval, ts::tsParser::location_type *location )

#define YY_NO_UNISTD_H

using token = ts::tsParser::token;
%}
  
%option debug
%option nodefault
%option yyclass="ts::tsScanner"
%option noyywrap
%option c++
%option yylineno



int (-?[0-9]+)
float (-?[0-9]+"."[0-9]+)
string \"(\\.|[^"\\])*\"
identifier [A-Za-z][a-zA-Z0-9_]*
globalRef #" "*"ref"
globalIn #" "*"in"

%%

%{          
    yylval = lval;
%}

end              return 0;
"END"            return token::tstEND;
"string"         return token::tstDEF_STRING;
"float"          return token::tstDEF_FLOAT;
"bool"           return token::tstDEF_BOOL;
"int"            return token::tstDEF_INT;
{globalRef}      return token::tstGLOBAL_REF;
{globalIn}       return token::tstGLOBAL_IN;
{identifier}     {
                    yylval->build<std::string>(yytext);
                    return token::tstIDENTIFIER;
                 }
{string}         return token::tstCONST_STRING;
{float}          return token::tstCONST_FLOAT;
{int}            return token::tstCONST_INT;
\n               ;
"{"              return token::tstOPEN_CBRACKET;
"}"              return token::tstCLOSE_CBRACKET;
"["              return token::tstOPEN_BRACKET;
"]"              return token::tstCLOSE_BRACKET;
"("              return token::tstOPEN_PAREN;
")"              return token::tstCLOSE_PAREN;
";"              return token::tstEXP_END;
"="              return token::tstEQUAL;
"+"              return token::tstADD;
"-"              return token::tstSUB;
"*"              return token::tstMUL;
"/"              return token::tstDIV;
"<"              return token::tstLESS;
">"              return token::tstMORE;
"<="             return token::tstLESS_EQUAL;
">="             return token::tstMORE_EQUAL;
"&&"             return token::tstAND;
"||"             return token::tstOR;
"!"              return token::tstNOT;
[\t ]            //Leave empty
.                printf("Unhandled token: %s ", yytext);return -1;
%%
  
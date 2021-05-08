%skeleton "lalr1.cc"
%require "3.5"
%language "c++"
%debug 
%defines 
%define parse.error verbose
%define api.namespace {ts}
%define api.parser.class {tsParser}


%code requires{

   namespace ts {
      class tsCompiler;
      class tsScanner;
   }

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { tsScanner  &scanner  }
%parse-param { tsCompiler  &compiler  }

%{
#include "../src/ThunderScriptCompiler.h"
#include "FlexLexer.h"
#include <stdio.h>

#undef yylex
#define yylex scanner.yylex
%}

%define api.value.type variant
%define parse.assert


%start line
%token tstEND
%token tstTRUE
%token tstFALSE
%token <std::string> tstCONST_INT
%token <std::string> tstCONST_FLOAT
%token <std::string> tstCONST_STRING
%token tstDEF_BOOL
%token tstDEF_INT
%token tstDEF_FLOAT
%token tstDEF_STRING
%token tstOPEN_BRACKET "["
%token tstCLOSE_BRACKET "]"
%token tstOPEN_CBRACKET "{"
%token tstCLOSE_CBRACKET "}"
%token tstOPEN_PAREN "("
%token tstCLOSE_PAREN ")"
%token <std::string> tstIDENTIFIER
%token tstIF
%token tstWHILE
%token tstFOR
%token tstADD "+"
%token tstSUB "-"
%token tstMUL
%token tstDIV
%token tstLESS
%token tstMORE
%token tstEQUAL "="
%token tstLESS_EQUAL
%token tstMORE_EQUAL
%token tstAND
%token tstOR
%token tstNOT
%token tstEXP_END ";"
%token tstGLOBAL_REF
%token tstGLOBAL_IN


%type <size_t> expression	
%type <size_t> value
%type <size_t> expStart
%right "="
%left "+" "-"

%locations

%%

line            : statement line 
				| statement
				;

variable        : tstDEF_BOOL tstIDENTIFIER {compiler.generateVar($2, tsVarType::tsBool);}
				| tstDEF_BOOL tstIDENTIFIER expression {tsVar var = compiler.generateVar($2, tsVarType::tsBool);;}
				| tstDEF_INT tstIDENTIFIER {compiler.generateVar($2, tsVarType::tsInt);}
				| tstDEF_INT tstIDENTIFIER "=" expression {tsVar var = compiler.generateVar($2, tsVarType::tsInt); compiler.assignVar(var.varIndex, $4, scanner.lineno());}
				| tstDEF_FLOAT tstIDENTIFIER {compiler.generateVar($2, tsVarType::tsFloat);}
				| tstIDENTIFIER "=" expression {printf("Found assign expression "); compiler.assignVar(compiler.getVarIndex($1), $3, scanner.lineno());}
                ;


statement		: preprocessor 
				| flow 
				| variable ";" 
				| enterScope "{" line "}" exitScope {printf("Found scope");}
				| expression ";" 
				| tstEND ";" 
				| ";"
				;


flow			: if
				| while
				| for
				;

if              : tstIF "(" expression ")" statement 
				;

while           : tstWHILE "(" expression ")" statement 
				;

for             : tstFOR tstOPEN_PAREN expression tstCLOSE_PAREN statement 
				;

preprocessor	: tstGLOBAL_REF tstDEF_INT tstIDENTIFIER {compiler.generateGlobal($3, tsVarType::tsInt, tsGlobal::GlobalType::tsRef, scanner.lineno());}
				| tstGLOBAL_REF tstDEF_BOOL tstIDENTIFIER {compiler.generateGlobal($3, tsVarType::tsBool, tsGlobal::GlobalType::tsRef, scanner.lineno());}
				| tstGLOBAL_REF tstDEF_FLOAT tstIDENTIFIER {compiler.generateGlobal($3, tsVarType::tsFloat, tsGlobal::GlobalType::tsRef, scanner.lineno());}
				| tstGLOBAL_IN tstDEF_INT tstIDENTIFIER {compiler.generateGlobal($3, tsVarType::tsInt, tsGlobal::GlobalType::tsIn, scanner.lineno());}
				| tstGLOBAL_IN tstDEF_BOOL tstIDENTIFIER {compiler.generateGlobal($3, tsVarType::tsBool, tsGlobal::GlobalType::tsIn, scanner.lineno());}
				| tstGLOBAL_IN tstDEF_FLOAT tstIDENTIFIER {compiler.generateGlobal($3, tsVarType::tsFloat, tsGlobal::GlobalType::tsIn, scanner.lineno());}
				;

expStart		: value enterScope {$$ = $1;};

expression		: expStart "+" expression exitScope {$$ = compiler.add($1, $3, scanner.lineno());}
				| expStart "-" expression exitScope {$$ = compiler.sub($1, $3, scanner.lineno());}
				| value { $$ = $1;}
				;

value           : tstIDENTIFIER {printf("Found identifier\n");$$ = compiler.getVarIndex($1);}
				| tstCONST_INT {printf("completed const int\n");$$ = compiler.getConst($1, tsVarType::tsInt, scanner.lineno());}
				;

enterScope		: %empty {compiler.enterScope();}
exitScope		: %empty {compiler.exitScope();}

%%

void ts::tsParser::error(const location_type &l, const std::string &err_message){
	std::cout << "Error: " << err_message << " at line " << scanner.lineno() << std::endl;
}
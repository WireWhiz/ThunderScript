%require "3.2"
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
%token tstCONST_BOOL
%token tstCONST_INT
%token tstCONST_FLOAT
%token <std::string> tstCONST_STRING
%token tstDEF_BOOL
%token tstDEF_INT
%token tstDEF_FLOAT
%token tstDEF_STRING
%token tstOPEN_BRACKET
%token tstCLOSE_BRACKET
%token tstOPEN_CBRACKET
%token tstCLOSE_CBRACKET
%token tstOPEN_PAREN
%token tstCLOSE_PAREN
%token <std::string> tstIDENTIFIER
%token tstIF
%token tstWHILE
%token tstADD
%token tstSUB
%token tstMUL
%token tstDIV
%token tstLESS
%token tstMORE
%token tstEQUAL
%token tstLESS_EQUAL
%token tstMORE_EQUAL
%token tstAND
%token tstOR
%token tstNOT
%token tstEXP_END
%token tstGLOBAL_REF
%token tstGLOBAL_IN

%locations

%%

line : statement line 
	 | statement
	 ;

statement		: preprocessor 
				| expression tstEXP_END
				| tstEND tstEXP_END {printf("found end");}
				| tstEXP_END
				;

preprocessor	: tstGLOBAL_REF tstDEF_INT tstIDENTIFIER {compiler.generateGlobal($3, tsValueType::tsInt, tsGlobal::GlobalType::tsRef, scanner.lineno());}
				| tstGLOBAL_REF tstDEF_BOOL tstIDENTIFIER {compiler.generateGlobal($3, tsValueType::tsBool, tsGlobal::GlobalType::tsRef, scanner.lineno());}
				| tstGLOBAL_REF tstDEF_FLOAT tstIDENTIFIER {compiler.generateGlobal($3, tsValueType::tsFloat, tsGlobal::GlobalType::tsRef, scanner.lineno());}
				| tstGLOBAL_IN tstDEF_INT tstIDENTIFIER {compiler.generateGlobal($3, tsValueType::tsInt, tsGlobal::GlobalType::tsIn, scanner.lineno());}
				| tstGLOBAL_IN tstDEF_BOOL tstIDENTIFIER {compiler.generateGlobal($3, tsValueType::tsBool, tsGlobal::GlobalType::tsIn, scanner.lineno());}
				| tstGLOBAL_IN tstDEF_FLOAT tstIDENTIFIER {compiler.generateGlobal($3, tsValueType::tsFloat, tsGlobal::GlobalType::tsIn, scanner.lineno());}
				;

expression		: tstIF tstOPEN_PAREN tstCLOSE_PAREN statement
				;

%%

void ts::tsParser::error(const location_type &l, const std::string &err_message){
	std::cout << "Error: " << err_message << " at line " << scanner.lineno() << std::endl;
}
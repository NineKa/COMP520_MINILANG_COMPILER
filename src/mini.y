%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "memory.h"
#include "treeNode.h"

extern int _lineNumber;
extern bool _valid;
extern miniTreeNode* _program;
extern memoryList _memlist;

void yyerror(const char* _what) {
	char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
    fprintf(stderr,"\e[31;1m[Error]\e[0m at line %d, %s\n", _lineNumber, _what);
	return;
}

int yylex();
%}

%union {
    int      intVal;
    char*    stringVal;
    float    flaotVal;
	struct miniTreeNode* treeNode;
}

%token <intVal>    tINT_LITERIAL
%token <stringVal> tSTRING_LITERIAL
%token <flaotVal>  tFLOAT_LITERIAL
%token <stringVal> tIDENTIFIER
%token tVAR tFLOAT tINT tSTRING tREAD tPRINT tWHILE tDO tDONE
%token tIF tTHEN tELSE tENDIF
	
%type <treeNode> Type Declaration Expression Statement Statement_List
%type <treeNode> Declaration_List MINI_PROGRAM

%start    MINI_PROGRAM

%left '+' '-'
%left '*' '/'

%%
	Type : tINT 	{$$ = newIntType();}
		 | tFLOAT	{$$ = newFloatType();}
		 | tSTRING	{$$ = newStringType();}
		 ;
			 
Declaration : tVAR tIDENTIFIER ':' Type ';' {$$ = newDeclaration(newIdentifier($2), $4);}
			;

Expression : Expression '+' Expression {$$ = newExpPlus($1, $3);}
	       | Expression '-' Expression {$$ = newExpMinus($1, $3);}
		   | Expression '*' Expression {$$ = newExpTimes($1, $3);}
		   | Expression '/' Expression {$$ = newExpDivide($1, $3);}
		   | '(' Expression ')'		   {$$ = $2;}
		   | '-' Expression            {$$ = newExpUniaryMinus($2);}
		   | tIDENTIFIER			   {$$ = newIdentifier($1);}
		   | tINT_LITERIAL			   {$$ = newIntLiterial($1);}
		   | tFLOAT_LITERIAL  		   {$$ = newFloatLiterial($1);}
		   | tSTRING_LITERIAL          {$$ = newStringLiterial($1);}
		   ;

Statement : tIDENTIFIER '=' Expression ';' 	{$$ = newStateAssign(newIdentifier($1), $3);}
		  | tPRINT Expression ';'			{$$ = newStatePrint($2);}
		  | tREAD Expression ';'			{$$ = newStateRead($2);}
		  | tIF Expression tTHEN Statement_List tENDIF						{$$ = newStateIf($2, $4);}
		  | tIF Expression tTHEN Statement_List tELSE Statement_List tENDIF {$$ = newStateIfElse($2, $4, $6);}
		  | tWHILE Expression tDO Statement_List tDONE						{$$ = newStateWhile($2, $4);}
		  ;
		  
Statement_List :							{$$ = NULL;} 
               | Statement Statement_List	{$$ = newStateList($1, $2);}
			   ;

Declaration_List :								{$$ = NULL;}
				 | Declaration Declaration_List	{$$ = newDefList($1, $2);} 
				 ;
				 
MINI_PROGRAM : Declaration_List Statement_List	{_program = newMiniProgram($1, $2);}
			 ;
%%
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "treeNode.h"
#include "memory.h"

#define ERROR_BUFFER	1024
void throwError(const char* _what);

#include "parser.h"
#include "prettyPrint.h"
#include "symTable.h"
#include "typeChecking.h"
#ifdef ASM
#include "codeGenerateASM.h"
#else
#include "codeGenerate.h"
#endif

miniTreeNode* _program;
int _lineNumber = 1;
FILE* _outputStream = NULL;
memoryList _memlist; 
symbolTable _table;
const char* _filename = NULL;
%}

%%
"#"[^\n]*        {/*COMMENT IGNORE*/}
[ \t]+           {/*WHITESPACE IGNORE*/}
(\r\n)|(\n)      {++_lineNumber;}
var              {return tVAR;}
":"              {return ':';}
float            {return tFLOAT;}
int              {return tINT;}
string           {return tSTRING;}
";"              {return ';';}
read             {return tREAD;}
print            {return tPRINT;}
while            {return tWHILE;}
do               {return tDO;}
done             {return tDONE;}
if               {return tIF;}
then             {return tTHEN;}
else             {return tELSE;}
endif            {return tENDIF;}
"+"              {return '+';}
"-"              {return '-';}
"*"              {return '*';}
"/"              {return '/';}
"("              {return '(';}
")"              {return ')';}
"="              {return '=';}

((0|([1-9][0-9]*))"."([0-9]*))|("."[0-9]+) {
                    yylval.flaotVal = atof(yytext);
                    return tFLOAT_LITERIAL;
                }

0|([1-9][0-9]*) {
                    yylval.intVal = atoi(yytext);
                    return tINT_LITERIAL;
					
                }

"\""[a-zA-Z0-9"."",""?""!"" "]*"\"" {
                    yylval.stringVal = (char*) mallocList (sizeof(char)*(strlen(yytext)+1), _memlist);
                    strcpy(yylval.stringVal, yytext);
                    return tSTRING_LITERIAL;
                }

[a-zA-Z_][a-zA-Z0-9_]* {
                    yylval.stringVal = (char*) mallocList (sizeof(char)*(strlen(yytext)+1), _memlist);
                    strcpy(yylval.stringVal, yytext);
                    return tIDENTIFIER;
                }

.                {return (char)yytext[0];}
%%
#include "mainFunc.c"
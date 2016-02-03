#ifndef __MINI__CODEGENERATE__H__
#define __MINI__CODEGENERATE__H__

#include "treeNode.h"
#include "symTable.h"

extern memoryList    _memlist;
extern symbolTable   _table;
extern FILE*		 _outputStream;
extern char*		 _filename;

size_t _codeGenerateLineIndent = 0L;

void generateVarDeclare(miniTreeNode* _var){
	symLink* varEntry = getSymbol(_table, _var);
	miniTreeNode* varNode = varEntry->definedAt;
	size_t lineNumber = varNode->lineNo;
	if (_filename != NULL){
		fprintf(_outputStream, "#line %zu \"%s\"\n", lineNumber, _filename);
	} else {
		fprintf(_outputStream, "#line %zu \n", lineNumber);
	}
	for (size_t iter = 0; iter < _codeGenerateLineIndent; iter++)
		fprintf(_outputStream, "\t");
	switch (varEntry->symbolType) {
		case SYMTYPE_INT : {
			fprintf(_outputStream,"long %s = 0L;\t//In script: %s\n",varEntry->generateName, varEntry->symbolName);
			break;
		}
		case SYMTYPE_FLOAT : {
			fprintf(_outputStream,"double %s = 0.0;\t//In script: %s\n",varEntry->generateName, varEntry->symbolName);
			break;
		}
		case SYMTYPE_STRING : {
			fprintf(_outputStream,"char* %s = \"\";\t//In script: %s\n",varEntry->generateName, varEntry->symbolName);
			break;
		}
	}
	return;
}
void generateDeclareList(miniTreeNode* _varlist){
	if (_varlist == NULL) return;
	miniTreeNode* currentDef = _varlist->nodeValue.def_list.def;
	miniTreeNode* nextPtr =    _varlist->nodeValue.def_list.next;
	generateVarDeclare(currentDef->nodeValue.declaration.id);
	generateDeclareList(nextPtr);
	return;
}

void generate(miniTreeNode* _program){
	generateDeclareList(_program->nodeValue.program.defList);
	//TODO
	return;
}

#endif
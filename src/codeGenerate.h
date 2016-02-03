#ifndef __MINI__CODEGENERATE__H__
#define __MINI__CODEGENERATE__H__

#include "treeNode.h"
#include "symTable.h"

extern memoryList    _memlist;
extern symbolTable   _table;
extern FILE*		 _outputStream;
extern char*		 _filename;

size_t _codeGenerateLineIndent = 0L;

void injectLibrary(){
	const char* memoryAllocateLibrary = 
		"struct memoryNode {\n"
		"	void*				ptr;\n"
		"	size_t				size;\n"
		"	struct memoryNode* 	next;\n"
		"};\n"
		"typedef struct memoryNode* memoryList;\n"
		"void freeList(memoryList);\n"
		"void* mallocList(size_t, memoryList);\n"
		"size_t sizeList(memoryList);\n"
		"memoryList newMemoryList();\n"
		"\n"
		"void freeList(memoryList _memList){\n"
		"	while (_memList != NULL) {\n"
		"		struct memoryNode* target = _memList;\n"
		"		_memList= _memList->next;\n"
		"		if (target->ptr != NULL) free(target->ptr);\n"
		"		target->ptr = NULL;\n"
		"		free(target);\n"
		"	}\n"
		"	free(_memList);\n"
		"}\n"
		"void* mallocList(size_t _size, memoryList _memList){\n"
		"	if (_memList == NULL) return NULL;\n"
		"	struct memoryNode* currentNode = (struct memoryNode*)malloc(sizeof(struct memoryNode));\n"
		"	if (currentNode == NULL) return NULL;\n"
		"	(currentNode->ptr) = malloc(_size);\n"
		"	(currentNode->size) = _size;\n"
		"	(currentNode->next) = NULL;\n"
		"	struct memoryNode* lastNode = _memList;\n"
		"	while (lastNode->next != NULL) lastNode = lastNode->next;\n"
		"	lastNode->next = currentNode;\n"
		"	return currentNode->ptr;\n"
		"}\n"
		"size_t sizeList(memoryList _list){\n"
		"	size_t cum = 0L;\n"
		"	while (_list != NULL){\n"
		"		cum = cum + (_list->size);\n"
		"		_list = (_list->next);\n"
		"	}\n"
		"	return cum;\n"
		"}	\n"
		"memoryList newMemoryList() {\n"
		"	memoryList returnList = (struct memoryNode*)malloc(sizeof(struct memoryNode));\n"
		"	(returnList->ptr) = NULL;\n"
		"	(returnList->size) = 0L;\n"
		"	(returnList->next) = NULL;\n"
		"	return returnList;\n"
		"}\n"
		"memoryList _memlist = NULL;\n";
	
	const char* stringConcatLibrary = 
		"char* concatString(const char* _str1, const char* _str2){\n"
		"	size_t strsize_1 = strlen(_str1) * sizeof(char) + sizeof(char);\n"
		"	size_t strsize_2 = strlen(_str2) * sizeof(char) + sizeof(char);\n"
		"	char* returnStr = (char*)mallocList(strsize_1 + strsize_2, _memlist);\n"
		"	strcpy(returnStr, _str1);\n"
		"	strcat(returnStr, _str2);\n"
		"	return returnStr;\n"
		"}\n";
	
	const char* stringReverseLibrary = 
		"char* reverseString(const char* _str1){\n"
		"	size_t strsize = strlen(_str1) * sizeof(char)+sizeof(char);\n"
		"	char* returnStr = (char*)mallocList(strsize, _memlist);\n"
		"	memset(returnStr, \'\\0\', strsize);\n"
		"	for (long iter = (long)strlen(_str1)-1; iter >= 0L; iter--){\n"
		"		returnStr[strlen(_str1)-iter-1] = _str1[iter];\n"
		"	}\n"
		"	return returnStr;\n"
		"}\n";

	const char* stringScanningLibrary = 
		"#define MAX_INPUT_BUFFER_LENGTH 1024\n"
		"char* scanString(){\n"
		"	char* returnPtr = (char*)mallocList(sizeof(char)*MAX_INPUT_BUFFER_LENGTH, _memlist);\n"
		"	scanf(\"%s\", returnPtr);\n"
		"	return returnPtr;\n"
		"}\n";
	
	fprintf(_outputStream,"%s",memoryAllocateLibrary);
	fprintf(_outputStream,"%s",stringConcatLibrary);
	fprintf(_outputStream,"%s",stringReverseLibrary);
	fprintf(_outputStream,"%s",stringScanningLibrary);
	return;
}

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

void generateExpression(miniTreeNode* _expr){
	switch (_expr -> nodeType) {
		case INT_LITERIAL : {
			fprintf(_outputStream, "%ldL", _expr->nodeValue.intLiterial);
			return;
		}
		case STRING_LITERIAL : {
			fprintf(_outputStream,"%s" , _expr->nodeValue.stringLiterial);
			return;
		}
		case FLOAT_LITERIAL : {
			fprintf(_outputStream, "%f", _expr->nodeValue.floatLiterial);
			return;
		}
		case IDENTIFIER : {
			symLink* varEntry = getSymbol(_table, _expr);
			fprintf(_outputStream, "%s", varEntry->generateName);
			return;
		}
		case EXPRESSION_PLUS : {
			miniTreeNode* leftNode = _expr->nodeValue.exp_plus.left;
			miniTreeNode* rightNode = _expr->nodeValue.exp_plus.right;
			typeCheckType typeResult = oracleExpression(_table, leftNode);
			if (typeResult == TYPECHECK_STRING){
				fprintf(_outputStream,"concatString(");
				generateExpression(leftNode);
				fprintf(_outputStream,",");
				generateExpression(rightNode);
				fprintf(_outputStream,")");
				return;
			}
			fprintf(_outputStream,"(");
			generateExpression(leftNode);
			fprintf(_outputStream,"+");
			generateExpression(rightNode);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_MINUS : {
			miniTreeNode* leftNode = _expr->nodeValue.exp_minus.left;
			miniTreeNode* rightNode = _expr->nodeValue.exp_minus.right;
			typeCheckType typeResult = oracleExpression(_table, leftNode);
			if (typeResult == TYPECHECK_STRING){
				fprintf(_outputStream,"concatString(");
				generateExpression(leftNode);
				fprintf(_outputStream,",reverseString(");
				generateExpression(rightNode);
				fprintf(_outputStream,"))");
				return;
			}
			fprintf(_outputStream,"(");
			generateExpression(leftNode);
			fprintf(_outputStream,"-");
			generateExpression(rightNode);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_TIMES : {
			miniTreeNode* leftNode = _expr->nodeValue.exp_times.left;
			miniTreeNode* rightNode = _expr->nodeValue.exp_times.right;
			fprintf(_outputStream,"(");
			generateExpression(leftNode);
			fprintf(_outputStream,"*");
			generateExpression(rightNode);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_DIVIDE : {
			miniTreeNode* leftNode = _expr->nodeValue.exp_divide.left;
			miniTreeNode* rightNode = _expr->nodeValue.exp_divide.right;
			fprintf(_outputStream,"(");
			generateExpression(leftNode);
			fprintf(_outputStream,"/");
			generateExpression(rightNode);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_UNIARY_MINUS : {
			miniTreeNode* expr = _expr->nodeValue.exp_uniary_minus.expr;
			typeCheckType typeResult = oracleExpression(_table, expr);
			if (typeResult == TYPECHECK_STRING){
				fprintf(_outputStream,"reverseString(");
				generateExpression(expr);
				fprintf(_outputStream,")");
				return;
			}
			fprintf(_outputStream,"(-(");
			generateExpression(expr);
			fprintf(_outputStream,"))");
			return;
		}
		default : return;
	}
}
void generateStatementList(miniTreeNode*);
void generateStatement(miniTreeNode* _state){
	size_t lineNumber = _state->lineNo;
	if (_filename != NULL){
		fprintf(_outputStream, "#line %zu \"%s\"\n", lineNumber, _filename);
	} else {
		fprintf(_outputStream, "#line %zu \n", lineNumber);
	}
	for (size_t iter = 0; iter < _codeGenerateLineIndent; iter++)
		fprintf(_outputStream, "\t");
	switch (_state->nodeType){
		case STATEMENT_ASSIGN : {
			miniTreeNode* idNode = _state->nodeValue.state_assign.id;
			miniTreeNode* exprNode = _state->nodeValue.state_assign.expr;
			generateExpression(idNode);
			fprintf(_outputStream," = ");
			generateExpression(exprNode);
			fprintf(_outputStream,";\n");
			return;
		}
		case STATEMENT_PRINT : {
			miniTreeNode* exprNode = _state->nodeValue.state_print.expr;
			typeCheckType typeResult = oracleExpression(_table, exprNode);
			switch (typeResult){
				case TYPECHECK_INT : {
					fprintf(_outputStream,"printf(\"%%ld\\n\",");
					generateExpression(exprNode);
					fprintf(_outputStream,");\n");
					return;
				}
				case TYPECHECK_FLOAT : {
					fprintf(_outputStream,"printf(\"%%f\\n\",");
					generateExpression(exprNode);
					fprintf(_outputStream,");\n");
					return;
				}
				case TYPECHECK_STRING : {
					fprintf(_outputStream,"printf(\"%%s\\n\",");
					generateExpression(exprNode);
					fprintf(_outputStream,");\n");
					return;
				}
				case TYPECHECK_INVALID : return;
			}
		}
		case STATEMENT_READ : {
			miniTreeNode* exprNode = _state->nodeValue.state_read.expr;
			typeCheckType typeResult = oracleExpression(_table, exprNode);
			switch (typeResult){
				case TYPECHECK_INT : {
					fprintf(_outputStream,"scanf(\"%%ld\",&");
					generateExpression(exprNode);
					fprintf(_outputStream,");\n");
					return;
				}
				case TYPECHECK_FLOAT : {
					fprintf(_outputStream,"scanf(\"%%f\",&");
					generateExpression(exprNode);
					fprintf(_outputStream,");\n");
					return;
				}
				case TYPECHECK_STRING : {
					generateExpression(exprNode);
					fprintf(_outputStream," = scanString();\n");
					return;
				}
				case TYPECHECK_INVALID : return;
			}
		}
		case STATEMENT_IF : {
			miniTreeNode* testNode = _state->nodeValue.state_if.expr;
			miniTreeNode* stateNode = _state->nodeValue.state_if.state;
			fprintf(_outputStream,"if (");
			generateExpression(testNode);
			fprintf(_outputStream,"){\n");
			_codeGenerateLineIndent++;
			generateStatementList(stateNode);
			_codeGenerateLineIndent--;
			for (size_t iter = 0; iter < _codeGenerateLineIndent; iter++)
				fprintf(_outputStream, "\t");
			fprintf(_outputStream,"}\n");
			return;
		}
		case STATEMENT_IF_ELSE : {
			miniTreeNode* testNode = _state->nodeValue.state_if_else.expr;
			miniTreeNode* stateTrueNode = _state->nodeValue.state_if_else.state_true;
			miniTreeNode* stateFalseNode = _state->nodeValue.state_if_else.state_false;
			fprintf(_outputStream,"if (");
			generateExpression(testNode);
			fprintf(_outputStream,"){\n");
			_codeGenerateLineIndent++;
			generateStatementList(stateTrueNode);
			_codeGenerateLineIndent--;
			for (size_t iter = 0; iter < _codeGenerateLineIndent; iter++)
				fprintf(_outputStream, "\t");
			fprintf(_outputStream,"} else {\n");
			_codeGenerateLineIndent++;
			generateStatementList(stateFalseNode);
			_codeGenerateLineIndent--;
			for (size_t iter = 0; iter < _codeGenerateLineIndent; iter++)
				fprintf(_outputStream, "\t");
			fprintf(_outputStream,"}\n");
			return;
		}
		case STATAEMENT_WHILE : {
			miniTreeNode* testNode = _state->nodeValue.state_while.expr;
			miniTreeNode* stateNode = _state->nodeValue.state_while.state;
			fprintf(_outputStream, "while (");
			generateExpression(testNode);
			fprintf(_outputStream,"){\n");
			_codeGenerateLineIndent++;
			generateStatementList(stateNode);
			_codeGenerateLineIndent--;
			for (size_t iter = 0; iter < _codeGenerateLineIndent; iter++)
				fprintf(_outputStream, "\t");
			fprintf(_outputStream,"}\n");
			return;
		}
		default : return;
	}
}
void generateStatementList(miniTreeNode* _statelist){
	if (_statelist == NULL) return;
	miniTreeNode* exprNode = _statelist->nodeValue.state_list.expr;
	miniTreeNode* nextNode = _statelist->nodeValue.state_list.next;
	generateStatement(exprNode);
	generateStatementList(nextNode);
	return;
}

void generate(miniTreeNode* _program){
	fprintf(_outputStream,"#include <stdio.h>\n");
	fprintf(_outputStream,"#include <stdlib.h>\n");
	fprintf(_outputStream,"#include <string.h>\n");
	injectLibrary();
	fprintf(_outputStream,"int main(int argc, char** argv) {\n");
	fprintf(_outputStream,"\t_memlist = newMemoryList();\n");
	_codeGenerateLineIndent = 1L;
	generateDeclareList(_program->nodeValue.program.defList);
	generateStatementList(_program->nodeValue.program.stateList);
	fprintf(_outputStream,"\tfreeList(_memlist);\n");
	fprintf(_outputStream,"\treturn 0;\n}\n");
	return;
}

#endif
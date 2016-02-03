#ifndef __MINI__TREE__H__
#define __MINI__TREE__H__

#include <stdio.h>
#include "memory.h"

extern memoryList _memlist;
extern int _lineNumber;

typedef enum {
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_STRING,
	
	INT_LITERIAL, 
	STRING_LITERIAL, 
	FLOAT_LITERIAL, 
	IDENTIFIER,
	
	DECLARATION, 
	EXPRESSION_PLUS, 
	EXPRESSION_MINUS,
	EXPRESSION_TIMES, 
	EXPRESSION_DIVIDE,
	EXPRESSION_UNIARY_MINUS,
	
	STATEMENT_ASSIGN,
	STATEMENT_PRINT,
	STATEMENT_READ,
	STATEMENT_IF,
	STATEMENT_IF_ELSE,
	STATAEMENT_WHILE,
	
	STATEMENT_LIST,
	DECLARATION_LIST,
	
	MINI_PROGRAM
} nodeType;

typedef struct miniTreeNode {
	size_t lineNo;
	nodeType nodeType;
	union{
		long  intLiterial;
		char* stringLiterial;
		double floatLiterial;
		char* identifier;
		
		struct {struct miniTreeNode* id; struct miniTreeNode* type;} declaration;
		struct {struct miniTreeNode* left; struct miniTreeNode* right;} exp_plus;
		struct {struct miniTreeNode* left; struct miniTreeNode* right;} exp_minus;
		struct {struct miniTreeNode* left; struct miniTreeNode* right;} exp_times;
		struct {struct miniTreeNode* left; struct miniTreeNode* right;} exp_divide;
		struct {struct miniTreeNode* expr;} exp_uniary_minus;
		struct {struct miniTreeNode* id; struct miniTreeNode* expr;} state_assign;
		struct {struct miniTreeNode* expr;} state_print;
		struct {struct miniTreeNode* expr;} state_read;
		struct {struct miniTreeNode* expr; struct miniTreeNode* state;} state_if;
		struct {struct miniTreeNode* expr; 
			    struct miniTreeNode* state_true;
				struct miniTreeNode* state_false;} state_if_else;
		struct {struct miniTreeNode* expr; struct miniTreeNode* state;} state_while;
		
		struct {struct miniTreeNode* expr; struct miniTreeNode* next;} state_list;
		struct {struct miniTreeNode* def; struct miniTreeNode* next;} def_list;
		struct {struct miniTreeNode* defList; struct miniTreeNode* stateList;} program;
	} nodeValue;
} miniTreeNode;

miniTreeNode* allocateNode(){
	miniTreeNode* returnNode = 
		(miniTreeNode*)mallocList(sizeof(miniTreeNode), _memlist);
	if (returnNode == NULL) {
		freeList(_memlist);
		printf("\e[31;1m[ERROR]\e[0m malloc error\n");
		abort();
	}
	return returnNode;
}

miniTreeNode* newIntLiterial(long _intLiterial){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = INT_LITERIAL;
	newNode->nodeValue.intLiterial = _intLiterial;
	return newNode;
}
miniTreeNode* newStringLiterial(const char* _stringLiterial){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STRING_LITERIAL;
	newNode->nodeValue.stringLiterial = (char*)_stringLiterial;
	return newNode;
}
miniTreeNode* newFloatLiterial(double _floatLiterial){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = FLOAT_LITERIAL;
	newNode->nodeValue.floatLiterial = _floatLiterial;
	return newNode;
}
miniTreeNode* newIdentifier(const char* _identifier){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = IDENTIFIER;
	newNode->nodeValue.identifier = (char*)_identifier;
	return newNode;
}
miniTreeNode* newDeclaration(struct miniTreeNode* _id, struct miniTreeNode* _type){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = DECLARATION;
	newNode->nodeValue.declaration.id = _id;
	newNode->nodeValue.declaration.type = _type;
	return newNode;
}
miniTreeNode* newExpPlus(struct miniTreeNode* _left, struct miniTreeNode* _right){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = EXPRESSION_PLUS;
	newNode->nodeValue.exp_plus.left = _left;
	newNode->nodeValue.exp_plus.right = _right;
	return newNode;
}
miniTreeNode* newExpMinus(struct miniTreeNode* _left, struct miniTreeNode* _right){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = EXPRESSION_MINUS;
	newNode->nodeValue.exp_minus.left = _left;
	newNode->nodeValue.exp_minus.right = _right;
	return newNode;
}
miniTreeNode* newExpTimes(struct miniTreeNode* _left, struct miniTreeNode* _right){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = EXPRESSION_TIMES;
	newNode->nodeValue.exp_times.left = _left;
	newNode->nodeValue.exp_times.right = _right;
	return newNode;
}
miniTreeNode* newExpDivide(struct miniTreeNode* _left, struct miniTreeNode* _right){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = EXPRESSION_DIVIDE;
	newNode->nodeValue.exp_divide.left = _left;
	newNode->nodeValue.exp_divide.right = _right;
	return newNode;
}
miniTreeNode* newExpUniaryMinus(struct miniTreeNode* _expr){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = EXPRESSION_UNIARY_MINUS;
	newNode->nodeValue.exp_uniary_minus.expr = _expr;
	return newNode;
}
miniTreeNode* newStateAssign(struct miniTreeNode* _id, struct miniTreeNode* _expr){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATEMENT_ASSIGN;
	newNode->nodeValue.state_assign.id = _id;
	newNode->nodeValue.state_assign.expr = _expr;
	return newNode;
}
miniTreeNode* newStatePrint(struct miniTreeNode* _expr){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATEMENT_PRINT;
	newNode->nodeValue.state_print.expr = _expr;
	return newNode;
}
miniTreeNode* newStateRead(struct miniTreeNode* _expr){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATEMENT_READ;
	newNode->nodeValue.state_read.expr = _expr;
	return newNode;
}
miniTreeNode* newStateIf(struct miniTreeNode* _expr, struct miniTreeNode* _state){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATEMENT_IF;
	newNode->nodeValue.state_if.expr = _expr;
	newNode->nodeValue.state_if.state = _state;
	return newNode;
}
miniTreeNode* newStateIfElse(struct miniTreeNode* _expr, struct miniTreeNode* _state_true, struct miniTreeNode* _state_false){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATEMENT_IF_ELSE;
	newNode->nodeValue.state_if_else.expr = _expr;
	newNode->nodeValue.state_if_else.state_true = _state_true;
	newNode->nodeValue.state_if_else.state_false = _state_false;
	return newNode;
}
miniTreeNode* newStateWhile(struct miniTreeNode* _expr, struct miniTreeNode* _state){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATAEMENT_WHILE;
	newNode->nodeValue.state_while.expr = _expr;
	newNode->nodeValue.state_while.state = _state;
	return newNode;
}
miniTreeNode* newStateList(struct miniTreeNode* _expr, struct miniTreeNode* _next){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = STATEMENT_LIST;
	newNode->nodeValue.state_list.expr = _expr;
	newNode->nodeValue.state_list.next = _next;
	return newNode;
}
miniTreeNode* newDefList(struct miniTreeNode* _def, struct miniTreeNode* _next){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = DECLARATION_LIST;
	newNode->nodeValue.def_list.def = _def;
	newNode->nodeValue.def_list.next = _next;
	return newNode;
}

miniTreeNode* newIntType(){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = TYPE_INT;
	return newNode;
}
miniTreeNode* newFloatType(){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = TYPE_FLOAT;
	return newNode;
}
miniTreeNode* newStringType(){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = TYPE_STRING;
	return newNode;
}

miniTreeNode* newMiniProgram(struct miniTreeNode* _defList, struct miniTreeNode* _stateList){
	miniTreeNode* newNode = allocateNode();
	newNode->lineNo = _lineNumber;
	newNode->nodeType = MINI_PROGRAM;
	newNode->nodeValue.program.defList = _defList;
	newNode->nodeValue.program.stateList = _stateList;
	return newNode;
}
#endif
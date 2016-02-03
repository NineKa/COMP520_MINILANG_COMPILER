#ifndef __MINI__SYMTABLE__H__
#define __MINI__SYMTABLE__H__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "memory.h"
#include "treeNode.h"

#define HASH_TABLE_SIZE 		100
#define VARIABLE_NAME_BUFFER	50

extern memoryList _memlist;
extern FILE* _outputStream;

size_t _generateVariableCounter = 0L;

typedef enum {
	SYMTYPE_INT,
	SYMTYPE_FLOAT,
	SYMTYPE_STRING,
} symType;

typedef struct symLink{
	char* 			symbolName;
	char* 			generateName;
	symType			symbolType;
	bool			used;
	bool 			inited;
	miniTreeNode*	definedAt;
	struct symLink*	next;
} symLink;

typedef symLink** symbolTable;
bool definedSymbol(symbolTable, miniTreeNode*);
symLink* getSymbol(symbolTable, miniTreeNode*);
void generateName(symLink*);

symbolTable newSymbolTable(){
	symbolTable returnTable =
		(symLink**)mallocList(sizeof(symLink*)*HASH_TABLE_SIZE, _memlist);
	for (size_t iter = 0; iter < HASH_TABLE_SIZE; iter++){
		returnTable[iter] = NULL;
	}
	return returnTable;
}
size_t symHash(const char* _identifier){
	size_t accHash = 0L;
	while (*_identifier != '\0'){
		accHash = (accHash << 1) + (*_identifier++);
	}
	return accHash % HASH_TABLE_SIZE;
}
void insertSymbol(symbolTable _table, miniTreeNode* _identifier, symType _type){
	if (definedSymbol(_table, _identifier) == true){
		symLink* definedSym = getSymbol(_table, _identifier);
		char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER,_memlist);
		memset(buffer, '\0', sizeof(char)*ERROR_BUFFER);
		sprintf(buffer, "identifier \e[1m%s\e[0m at line %zu has been defined at line %zu\n",
				_identifier->nodeValue.identifier,
				_identifier->lineNo,
				definedSym->definedAt->lineNo);
		throwError(buffer);
		abort();
	}
	size_t hashCode = symHash(_identifier->nodeValue.identifier);
	struct symLink* newSym = 
		(struct symLink*)mallocList(sizeof(struct symLink), _memlist);
	(newSym->symbolName) = _identifier->nodeValue.identifier;
	(newSym->symbolType) = _type;
	(newSym->definedAt) = _identifier;
	(newSym->used) = false;
	(newSym->inited) = false;
	generateName(newSym);
	(newSym->next) = NULL;
	if (_table[hashCode] == NULL){
		_table[hashCode] = newSym;
	} else {
		(newSym->next) = _table[hashCode];
		_table[hashCode] = newSym;
	}
	return;
}
bool definedSymbol(symbolTable _table, miniTreeNode* _identifier){
	size_t hashCode = symHash(_identifier->nodeValue.identifier);
	if (_table[hashCode] == NULL) return false;
	struct symLink* ptr = _table[hashCode];
	while (ptr != NULL){
		if (strcmp(ptr->symbolName, _identifier->nodeValue.identifier) == 0) return true;
		ptr = ptr->next;
	}
	return false;
}
symLink* getSymbol(symbolTable _table, miniTreeNode* _identifier){
	size_t hashCode = symHash(_identifier->nodeValue.identifier);
	if (_table[hashCode] == NULL) {
		char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER,_memlist);
		memset(buffer, '\0', sizeof(char)*ERROR_BUFFER);
		sprintf(buffer, "identifier \e[1m%s\e[0m at line %zu not defined\n",
				_identifier->nodeValue.identifier,
				_identifier->lineNo);
		throwError(buffer);
		abort();
	}
	struct symLink* ptr = _table[hashCode];
	while (ptr != NULL){
		if (strcmp(ptr->symbolName, _identifier->nodeValue.identifier) == 0) 
			return ptr;
		ptr = ptr->next;
	}
	char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER,_memlist);
	memset(buffer, '\0', sizeof(char)*ERROR_BUFFER);
	sprintf(buffer, "identifier \e[1m%s\e[0m at line %zu not defined\n",
			_identifier->nodeValue.identifier,
			_identifier->lineNo);
	throwError(buffer);
	abort();
}
void generateName(symLink* _symbol){
	(_symbol->generateName) = (char*)mallocList(sizeof(char)*VARIABLE_NAME_BUFFER, _memlist);
	sprintf(_symbol->generateName, "t_%zu", _generateVariableCounter);
	_generateVariableCounter++;
}
symLink* exprFreeVariable(symbolTable _table, miniTreeNode* _expr){
	switch (_expr->nodeType){
		case EXPRESSION_PLUS : {
			symLink* result_1 = exprFreeVariable(_table, _expr->nodeValue.exp_plus.left);
			if (result_1 != NULL) return result_1;
			symLink* result_2 = exprFreeVariable(_table, _expr->nodeValue.exp_plus.right);
			if (result_2 != NULL) return result_2;
			return NULL;
		}
		case EXPRESSION_MINUS : {
			symLink* result_1 = exprFreeVariable(_table, _expr->nodeValue.exp_minus.left);
			if (result_1 != NULL) return result_1;
			symLink* result_2 = exprFreeVariable(_table, _expr->nodeValue.exp_minus.right);
			if (result_2 != NULL) return result_2;
			return NULL;
		}
		case EXPRESSION_TIMES : {
			symLink* result_1 = exprFreeVariable(_table, _expr->nodeValue.exp_times.left);
			if (result_1 != NULL) return result_1;
			symLink* result_2 = exprFreeVariable(_table, _expr->nodeValue.exp_times.right);
			if (result_2 != NULL) return result_2;
			return NULL;
		}
		case EXPRESSION_DIVIDE : {
			symLink* result_1 = exprFreeVariable(_table, _expr->nodeValue.exp_divide.left);
			if (result_1 != NULL) return result_1;
			symLink* result_2 = exprFreeVariable(_table, _expr->nodeValue.exp_divide.right);
			if (result_2 != NULL) return result_2;
			return NULL;
		}
		case EXPRESSION_UNIARY_MINUS : {
			symLink* result_1 = exprFreeVariable(_table, _expr->nodeValue.exp_uniary_minus.expr);
			return result_1;
		}
		case IDENTIFIER : {
			symLink* identifierPtr = getSymbol(_table, _expr);
			if (identifierPtr->inited == false) return identifierPtr;
			return NULL;
		}
		default : return NULL;
	}
}
void generateSymbolTable(symbolTable _table, miniTreeNode* _node){
	if (_node == NULL) return;
 	switch (_node->nodeType) {
		case DECLARATION : {
			miniTreeNode* _typeNode = _node->nodeValue.declaration.type;
			miniTreeNode* _identiferNode = _node->nodeValue.declaration.id;
			switch (_typeNode->nodeType){
				case TYPE_INT : {
					insertSymbol(_table, _identiferNode, SYMTYPE_INT);
					break;
				}
				case TYPE_FLOAT : {
					insertSymbol(_table, _identiferNode, SYMTYPE_FLOAT);
					break;
				}
				case TYPE_STRING : {
					insertSymbol(_table, _identiferNode, SYMTYPE_STRING);
					break;
				}
				default : break;
			}
			break;
		}
		case DECLARATION_LIST : {
			generateSymbolTable(_table, _node->nodeValue.def_list.def);
			generateSymbolTable(_table, _node->nodeValue.def_list.next);
			break;
		}
		case MINI_PROGRAM : {
			generateSymbolTable(_table, _node->nodeValue.program.defList);
			generateSymbolTable(_table, _node->nodeValue.program.stateList);
			break;
		}
		case EXPRESSION_PLUS : {
			generateSymbolTable(_table, _node->nodeValue.exp_plus.left);
			generateSymbolTable(_table, _node->nodeValue.exp_plus.right);
			break;
		}
		case EXPRESSION_MINUS : {
			generateSymbolTable(_table, _node->nodeValue.exp_minus.left);
			generateSymbolTable(_table, _node->nodeValue.exp_minus.right);
			break;
		}
		case EXPRESSION_TIMES : {
			generateSymbolTable(_table, _node->nodeValue.exp_times.left);
			generateSymbolTable(_table, _node->nodeValue.exp_times.right);
			break;
		}
		case EXPRESSION_DIVIDE : {
			generateSymbolTable(_table, _node->nodeValue.exp_divide.left);
			generateSymbolTable(_table, _node->nodeValue.exp_divide.right);
			break;
		}
		case EXPRESSION_UNIARY_MINUS : {
			generateSymbolTable(_table, _node->nodeValue.exp_uniary_minus.expr);
			break;
		}
		case STATEMENT_ASSIGN : {
			miniTreeNode* identifierNode = _node->nodeValue.state_assign.id;
			bool isDefined = definedSymbol(_table, identifierNode);
			if (isDefined == false) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "identifier \e[1m%s\e[0m at line %zu is not defined\n",
						identifierNode->nodeValue.identifier,
						identifierNode->lineNo);
				throwError(buffer);
				abort(); 
			}
			symLink* sym = getSymbol(_table, identifierNode);
			(sym->used) = true;
			(sym->inited) = true;
			generateSymbolTable(_table, _node->nodeValue.state_assign.expr);
			break;
		}
		case STATEMENT_PRINT : {
			generateSymbolTable(_table, _node->nodeValue.state_print.expr);
			break;
			
		}
		case STATEMENT_READ : {
			miniTreeNode* identifierNode = _node->nodeValue.state_assign.id;
			bool isDefined = definedSymbol(_table, identifierNode);
			if (isDefined == false) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "identifier \e[1m%s\e[0m at line %zu is not defined\n",
						identifierNode->nodeValue.identifier,
						identifierNode->lineNo);
				throwError(buffer);
				abort(); 
			}
			symLink* sym = getSymbol(_table, identifierNode);
			(sym->used) = true;
			(sym->inited) = true;
			break;
		}
		case STATEMENT_IF : {
			generateSymbolTable(_table, _node->nodeValue.state_if.expr);
			generateSymbolTable(_table, _node->nodeValue.state_if.state);
			break;
		}
		case STATEMENT_IF_ELSE : {
			generateSymbolTable(_table, _node->nodeValue.state_if_else.expr);
			generateSymbolTable(_table, _node->nodeValue.state_if_else.state_true);
			generateSymbolTable(_table, _node->nodeValue.state_if_else.state_false);
			break;
		}
		case STATAEMENT_WHILE : {
			generateSymbolTable(_table, _node->nodeValue.state_while.expr);
			generateSymbolTable(_table, _node->nodeValue.state_while.state);
			break;
		}
		case STATEMENT_LIST : {
			generateSymbolTable(_table, _node->nodeValue.state_list.expr);
			generateSymbolTable(_table, _node->nodeValue.state_list.next);
			break;
		}
		case IDENTIFIER : {
			bool isDefined = definedSymbol(_table, _node);
			if (isDefined == false) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "identifier \e[1m%s\e[0m at line %zu is not defined\n",
						_node->nodeValue.identifier,
						_node->lineNo);
				throwError(buffer);
				abort(); 
			}
			symLink* sym = getSymbol(_table, _node);
			(sym->used) = true;
			if (sym->inited == false){
				fprintf(stderr, "\e[33;1m[Warning]\e[0m variable \e[1m%s\e[0m at line %zu is not initialized before use\n",
						_node->nodeValue.identifier,
						_node->lineNo);
			}
			break;
		}
		default: break;
	}
	return;
}
void printSymbolTable(symbolTable _table){
	fprintf(_outputStream, "Var Name\tVar Type\tAlign Name\tUsed\tInit\n");
	for (size_t iter = 0L; iter < HASH_TABLE_SIZE; iter++){
		if (_table[iter] == NULL) continue;
		symLink* ptr = _table[iter];
		while (ptr != NULL){
			fprintf(_outputStream, "%s\t", ptr->symbolName);
			switch (ptr->symbolType){
				case SYMTYPE_INT : {
					fprintf(_outputStream, "int\t");
					break;
				}
				case SYMTYPE_FLOAT : {
					fprintf(_outputStream, "float\t");
					break;
				}
				case SYMTYPE_STRING : {
					fprintf(_outputStream, "string\t");
					break;
				}
			}
			fprintf(_outputStream, "%s\t", ptr->generateName);
			if (ptr->used == true) {
				fprintf(_outputStream, "true \t");
			} else {
				fprintf(_outputStream, "false \t");
			}
			if (ptr->inited == true) {
				fprintf(_outputStream, "true \t");
			} else {
				fprintf(_outputStream, "false \t");
			}
			fprintf(_outputStream, "\n");
			ptr = ptr->next;
		}
	}
}
void scanSymbolTable(symbolTable _table){
	for (size_t iter = 0L; iter < HASH_TABLE_SIZE; iter++){
		if (_table[iter] == NULL) continue;
		symLink* ptr = _table[iter];
		while (ptr != NULL){
			if (ptr->used == false) {
				fprintf(stderr, "\e[33;1m[Warning]\e[0m variable \e[1m%s\e[0m defined at line %zu is never used\n",
						ptr->definedAt->nodeValue.identifier,
						ptr->definedAt->lineNo);
			}
			ptr = ptr -> next;
		}
	}
	return;
}
#endif
#ifndef __MINI__TYPECHECKING__H__
#define __MINI__TYPECHECKING__H__

#include "symTable.h"
#include <stdbool.h>

extern memoryList _memlist;

typedef enum{
	TYPECHECK_INT,
	TYPECHECK_FLOAT,
	TYPECHECK_STRING,
	TYPECHECK_INVALID
} typeCheckType;

// _A = _B
bool oracleAssign(typeCheckType _A, typeCheckType _B){
	switch (_A) {
		case TYPECHECK_INT : 
			switch (_B) {
				case TYPECHECK_INT :		return true;
				case TYPECHECK_FLOAT : 		return false;
				case TYPECHECK_STRING : 	return false;
				case TYPECHECK_INVALID :	return false;
			}
		case TYPECHECK_FLOAT :
			switch (_B) {
				case TYPECHECK_INT :		return true;
				case TYPECHECK_FLOAT : 		return true;
				case TYPECHECK_STRING :		return false;
				case TYPECHECK_INVALID : 	return false;
			}
		case TYPECHECK_STRING : 
			switch (_B) {
				case TYPECHECK_INT :		return false;
				case TYPECHECK_FLOAT :		return false;
				case TYPECHECK_STRING : 	return true;
				case TYPECHECK_INVALID :	return false;
			}
		case TYPECHECK_INVALID :			return false;
	}
}
typeCheckType oracleUniaryMinus(typeCheckType _A){
	switch (_A) {
		case TYPECHECK_INT :		return TYPECHECK_INT;
		case TYPECHECK_FLOAT :		return TYPECHECK_FLOAT;
		case TYPECHECK_STRING :		return TYPECHECK_STRING;
		case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
	}
}
typeCheckType oracleAdd(typeCheckType _A, typeCheckType _B){
	switch (_A) {
		case TYPECHECK_INT :
			switch (_B) {
				case TYPECHECK_INT : 		return TYPECHECK_INT;
				case TYPECHECK_FLOAT : 		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING : 	return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_FLOAT :
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_FLOAT;
				case TYPECHECK_FLOAT :		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING :		return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_STRING:
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_INVALID;
				case TYPECHECK_FLOAT :		return TYPECHECK_INVALID;
				case TYPECHECK_STRING :		return TYPECHECK_STRING;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_INVALID :			return TYPECHECK_INVALID;
	}
}
typeCheckType oracleMinus(typeCheckType _A, typeCheckType _B){
	switch (_A) {
		case TYPECHECK_INT :
			switch (_B) {
				case TYPECHECK_INT : 		return TYPECHECK_INT;
				case TYPECHECK_FLOAT : 		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING : 	return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_FLOAT :
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_FLOAT;
				case TYPECHECK_FLOAT :		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING :		return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_STRING:
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_INVALID;
				case TYPECHECK_FLOAT :		return TYPECHECK_INVALID;
				case TYPECHECK_STRING :		return TYPECHECK_STRING;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_INVALID :			return TYPECHECK_INVALID;
	}
}
typeCheckType oracleTimes(typeCheckType _A, typeCheckType _B){
	switch (_A) {
		case TYPECHECK_INT :
			switch (_B) {
				case TYPECHECK_INT : 		return TYPECHECK_INT;
				case TYPECHECK_FLOAT : 		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING : 	return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_FLOAT :
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_FLOAT;
				case TYPECHECK_FLOAT :		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING :		return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_STRING:
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_INVALID;
				case TYPECHECK_FLOAT :		return TYPECHECK_INVALID;
				case TYPECHECK_STRING :		return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_INVALID :			return TYPECHECK_INVALID;
	}
}
typeCheckType oracleDivide(typeCheckType _A, typeCheckType _B){
	switch (_A) {
		case TYPECHECK_INT :
			switch (_B) {
				case TYPECHECK_INT : 		return TYPECHECK_INT;
				case TYPECHECK_FLOAT : 		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING : 	return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_FLOAT :
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_FLOAT;
				case TYPECHECK_FLOAT :		return TYPECHECK_FLOAT;
				case TYPECHECK_STRING :		return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_STRING:
			switch (_B) {
				case TYPECHECK_INT :		return TYPECHECK_INVALID;
				case TYPECHECK_FLOAT :		return TYPECHECK_INVALID;
				case TYPECHECK_STRING :		return TYPECHECK_INVALID;
				case TYPECHECK_INVALID :	return TYPECHECK_INVALID;
			}
		case TYPECHECK_INVALID :			return TYPECHECK_INVALID;
	}
}

char* typeCheckType2Str(typeCheckType _type){
	char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
	switch (_type) {
		case TYPECHECK_INT : {
			sprintf(buffer, "int");
			return buffer;
		}
		case TYPECHECK_FLOAT : {
			sprintf(buffer, "float");
			return buffer;
		}
		case TYPECHECK_STRING : {
			sprintf(buffer, "string");
			return buffer;
		}
		case TYPECHECK_INVALID : {
			sprintf(buffer, "invalid");
			return buffer;
		}
	}
}

typeCheckType oracleExpression(symbolTable _table, miniTreeNode* _expr){
	switch (_expr->nodeType){
		case EXPRESSION_PLUS : {
			typeCheckType leftType   = oracleExpression(_table, _expr->nodeValue.exp_plus.left);
			typeCheckType rightType  = oracleExpression(_table, _expr->nodeValue.exp_plus.right);
			typeCheckType returnType = oracleAdd(leftType, rightType); 
			if (returnType == TYPECHECK_INVALID){
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of addition : %s + %s\n",
						_expr->lineNo,
						typeCheckType2Str(leftType),
						typeCheckType2Str(rightType));
				throwError(buffer);
				abort();
			}
			return returnType;
		}
		case EXPRESSION_MINUS : {
			typeCheckType leftType   = oracleExpression(_table, _expr->nodeValue.exp_minus.left);
			typeCheckType rightType  = oracleExpression(_table, _expr->nodeValue.exp_minus.right);
			typeCheckType returnType = oracleMinus(leftType, rightType);
			if (returnType == TYPECHECK_INVALID){
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of minus : %s - %s\n",
						_expr->lineNo,
						typeCheckType2Str(leftType),
						typeCheckType2Str(rightType));
				throwError(buffer);
				abort();
			}
			return returnType;
		}
		case EXPRESSION_TIMES : {
			typeCheckType leftType  = oracleExpression(_table, _expr->nodeValue.exp_times.left);
			typeCheckType rightType = oracleExpression(_table, _expr->nodeValue.exp_times.right);
			typeCheckType returnType = oracleTimes(leftType, rightType);
			if (returnType == TYPECHECK_INVALID){
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of multiplication : %s * %s\n",
						_expr->lineNo,
						typeCheckType2Str(leftType),
						typeCheckType2Str(rightType));
				throwError(buffer);
				abort();
			}
			return returnType;
		}
		case EXPRESSION_DIVIDE : {
			typeCheckType leftType  = oracleExpression(_table, _expr->nodeValue.exp_divide.left);
			typeCheckType rightType = oracleExpression(_table, _expr->nodeValue.exp_divide.right);
			typeCheckType returnType = oracleDivide(leftType, rightType);
			if (returnType == TYPECHECK_INVALID){
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of division : %s / %s\n",
						_expr->lineNo,
						typeCheckType2Str(leftType),
						typeCheckType2Str(rightType));
				throwError(buffer);
				abort();
			}
			return returnType;
		} 
		case EXPRESSION_UNIARY_MINUS : {
			typeCheckType exprType   = oracleExpression(_table, _expr->nodeValue.exp_uniary_minus.expr);
			typeCheckType returnType = oracleUniaryMinus(exprType);
			if (returnType == TYPECHECK_INVALID){
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of uniary minus : - %s\n",
						_expr->lineNo,
						typeCheckType2Str(exprType));
				throwError(buffer);
				abort();
			}
			return returnType;
		}
		case INT_LITERIAL 		:	return TYPECHECK_INT;
		case STRING_LITERIAL	:	return TYPECHECK_STRING;
		case FLOAT_LITERIAL 	:	return TYPECHECK_FLOAT;
		case IDENTIFIER			: {
			symLink* _symLink = getSymbol(_table, _expr);
			switch (_symLink->symbolType) {
				case SYMTYPE_INT		:	return TYPECHECK_INT;
				case SYMTYPE_FLOAT		:	return TYPECHECK_FLOAT;
				case SYMTYPE_STRING		:	return TYPECHECK_STRING;
				default					: 	return TYPECHECK_INVALID;
			}
		}
		default					:	break; 
	}
	return TYPECHECK_INVALID;
}
void oracleStatementList(symbolTable _table, miniTreeNode* _stateList){
	if (_stateList == NULL) return;
	miniTreeNode* exprNode = _stateList->nodeValue.state_list.expr;
	miniTreeNode* nextNode = _stateList->nodeValue.state_list.next;
	switch (exprNode -> nodeType) {
		case STATEMENT_ASSIGN : {
			miniTreeNode* id   = exprNode->nodeValue.state_assign.id;
			miniTreeNode* expr = exprNode->nodeValue.state_assign.expr;
			typeCheckType idType   = oracleExpression(_table, id);
			typeCheckType exprType = oracleExpression(_table, expr);
			if (oracleAssign(idType, exprType) == false) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of assignment : %s = %s\n",
						expr->lineNo,
						typeCheckType2Str(idType),
						typeCheckType2Str(exprType));
				throwError(buffer);
				abort();
			}
			oracleStatementList(_table, nextNode);
			return;
		}
		case STATEMENT_PRINT : {
			miniTreeNode* expr = exprNode->nodeValue.state_print.expr;
			typeCheckType exprType = oracleExpression(_table, expr);
			//REMAINDER : INT FLOAT STRING all avaliable for print
			oracleStatementList(_table, nextNode);
			return;
		}
		case STATEMENT_READ : {
			//REMAINDER : INT FLOAT STRING all avaliable for read
			oracleStatementList(_table, nextNode);
			return;
		}
		case STATEMENT_IF : {
			miniTreeNode* testExpr = exprNode->nodeValue.state_if.expr;
			miniTreeNode* stateExpr= exprNode->nodeValue.state_if.state;
			typeCheckType testType = oracleExpression(_table, testExpr);
			if (testType != TYPECHECK_INT) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of if : test expression must have type int, but has %s type\n",
						testExpr->lineNo,
						typeCheckType2Str(testType));
				throwError(buffer);
				abort();
			}
			oracleStatementList(_table, stateExpr);
			return;
		}
		case STATEMENT_IF_ELSE : {
			miniTreeNode* testExpr = exprNode->nodeValue.state_if_else.expr;
			miniTreeNode* stateTrueExpr  = exprNode->nodeValue.state_if_else.state_true;
			miniTreeNode* stateFalseExpr = exprNode->nodeValue.state_if_else.state_false;
			typeCheckType testType = oracleExpression(_table, testExpr);
			if (testType != TYPECHECK_INT) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of if : test expression must have type int, but has %s type\n",
						testExpr->lineNo,
						typeCheckType2Str(testType));
				throwError(buffer);
				abort();
			}
			oracleStatementList(_table, stateTrueExpr);
			oracleStatementList(_table, stateFalseExpr);
			return;
		}
		case STATAEMENT_WHILE : {
			miniTreeNode* testExpr = exprNode->nodeValue.state_while.expr;
			miniTreeNode* stateExpr = exprNode->nodeValue.state_while.state;
			typeCheckType testType = oracleExpression(_table, testExpr);
			if (testType != TYPECHECK_INT) {
				char* buffer = (char*)mallocList(sizeof(char)*ERROR_BUFFER, _memlist);
				sprintf(buffer, "at line %zu, invalid type of while : test expression must have type int, but has %s type\n",
						testExpr->lineNo,
						typeCheckType2Str(testType));
				throwError(buffer);
				abort();
			}
			oracleStatementList(_table, stateExpr);
			return;
		}
		default : 				return;
	}
}
void oracleProgram(symbolTable _table, miniTreeNode* _program){
	miniTreeNode* mainList = _program->nodeValue.program.stateList;
	oracleStatementList(_table, mainList);
	return;
}
#endif
#ifndef __MINI_CODEGENERATEASM_H__
#define __MINI_CODEGENERATEASM_H__

#include "treeNode.h"
#include "symTable.h"
#include "typeChecking.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define quickASM(_opCode,_args,_comment) fprintf(_outputStream,_opCode"\t"_args"\t;; "_comment"\n")

extern memoryList _memlist;
extern symbolTable _table;
extern FILE* _outputStream;
extern const char* _filename;

typedef struct constStrListType {
    char* strPtr;
    char* resID;
    struct constStrListType* next;
} constStrListType;

typedef struct constDoubleListType {
    double num;
    char* resID;
    struct constDoubleListType* next;
} constDoubleListType;

constStrListType* strManager = NULL;
constDoubleListType* doubleManager = NULL;
size_t strManagerLen = 0L;
size_t doubleManagerLen = 0L;

void putString(const char* _str) {
    // TODO : Error Check
    if (strManager == NULL) {
	strManager = (constStrListType*)malloc(sizeof(constStrListType));
	strManager->strPtr = (char*)malloc(sizeof(char)*(strlen(_str)+1));
	strManager->resID = (char*)malloc(sizeof(char)*256);
	strManager->next = NULL;
	strcpy(strManager->strPtr, _str);
	sprintf(strManager->resID, "str_%zu", strManagerLen);
	strManagerLen++;
	return;
    }
    constStrListType* ptr = strManager;
    while (ptr != NULL) {
	if (strcmp(ptr->strPtr, _str) == 0L) return;
	ptr = ptr->next;
    }
    ptr = strManager;
    while (ptr->next != NULL) ptr = ptr->next;
    ptr->next = (constStrListType*)malloc(sizeof(constStrListType));
    ptr = ptr->next;
    ptr->strPtr = (char*)malloc(sizeof(char)*(strlen(_str)+1));
    ptr->resID = (char*)malloc(sizeof(char)*256);
    ptr->next = NULL;
    strcpy(ptr->strPtr, _str);
    sprintf(ptr->resID, "str_%zu", strManagerLen);
    strManagerLen++;
    return;
}

void putDouble(double _num) {
    if (doubleManager == NULL) {
	doubleManager = (constDoubleListType*)malloc(sizeof(constDoubleListType));
	doubleManager->num = _num;
	doubleManager->resID = (char*)malloc(sizeof(char)*256);
	doubleManager->next = NULL;
	sprintf(doubleManager->resID, "double_%zu", doubleManagerLen);
	doubleManagerLen++;
	return;
    }
    constDoubleListType* ptr = doubleManager;
    while (ptr != NULL) {
	if (_num == ptr->num) return;
	ptr = ptr->next;
    }
    ptr = doubleManager;
    while (ptr->next != NULL) ptr = ptr->next;
    ptr->next = (constDoubleListType*)malloc(sizeof(constDoubleListType));
    ptr = ptr->next;
    ptr->num = _num;
    ptr->resID = (char*)malloc(sizeof(char)*256);
    ptr->next = NULL;
    sprintf(ptr->resID, "double_%zu", doubleManagerLen);
    doubleManagerLen++;
    return;
}

char* getString(const char* _str) {
    constStrListType* ptr = strManager;
    while (ptr != NULL) {
	if (strcmp(ptr->strPtr, _str) == 0L) return ptr->resID;
	ptr = ptr->next;
    }
    return NULL;
}
char* getDouble(double _num) {
    constDoubleListType* ptr = doubleManager;
    while (ptr != NULL) {
	if (_num == ptr->num) return ptr->resID;
	ptr = ptr->next;
    }
    return NULL;
}

void generateStrResExp(miniTreeNode* _expr) {
    assert(_expr != NULL);
    switch (_expr->nodeType) {
    case INT_LITERIAL: return;
    case STRING_LITERIAL: {
	putString(_expr->nodeValue.stringLiterial);
	return;
    }
    case FLOAT_LITERIAL: {
	putDouble(_expr->nodeValue.floatLiterial);
	return;
    };
    case IDENTIFIER: return;
    case EXPRESSION_PLUS: {
	generateStrResExp(_expr->nodeValue.exp_plus.left);
	generateStrResExp(_expr->nodeValue.exp_plus.right);
	return;
    }
    case EXPRESSION_MINUS: {
	generateStrResExp(_expr->nodeValue.exp_minus.left);
	generateStrResExp(_expr->nodeValue.exp_minus.right);
	return;
    }
    case EXPRESSION_TIMES: {
	generateStrResExp(_expr->nodeValue.exp_times.left);
	generateStrResExp(_expr->nodeValue.exp_times.right);
	return;
    }
    case EXPRESSION_DIVIDE: {
	generateStrResExp(_expr->nodeValue.exp_divide.left);
	generateStrResExp(_expr->nodeValue.exp_divide.right);
	return;
    }
    case EXPRESSION_UNIARY_MINUS: {
	generateStrResExp(_expr->nodeValue.exp_uniary_minus.expr);
    }
    default: return;
    }
}
void generateStrResStateList(miniTreeNode*);
void generateStrResState(miniTreeNode* _state) {
    assert(_state != NULL);
    switch (_state->nodeType) {
    case STATEMENT_ASSIGN: {
	generateStrResExp(_state->nodeValue.state_assign.expr);
	return;
    }
    case STATEMENT_PRINT: {
	generateStrResExp(_state->nodeValue.state_print.expr);
	return;
    }
    case STATEMENT_IF: {
	generateStrResExp(_state->nodeValue.state_if.expr);
	generateStrResStateList(_state->nodeValue.state_if.state);
	return;
    }
    case STATEMENT_IF_ELSE: {
	generateStrResExp(_state->nodeValue.state_if_else.expr);
	generateStrResStateList(_state->nodeValue.state_if_else.state_true);
	generateStrResStateList(_state->nodeValue.state_if_else.state_false);
	return;
    }
    case STATAEMENT_WHILE: {
	generateStrResExp(_state->nodeValue.state_while.expr);
	generateStrResStateList(_state->nodeValue.state_while.state);
	return;
    }
    default: return;
    }
}
void generateStrResStateList(miniTreeNode* _statelist){
    if (_statelist == NULL) return;
    generateStrResState(_statelist->nodeValue.state_list.expr);
    generateStrResStateList(_statelist->nodeValue.state_list.next);
    return;
}
void generateVarRes(miniTreeNode* _declare) {
    symLink* varEntry = getSymbol(_table, _declare->nodeValue.declaration.id);
    miniTreeNode* varNode = varEntry->definedAt;
    switch (varEntry->symbolType) {
    case SYMTYPE_INT: {
	fprintf(_outputStream, "%s:\t\tdq\t0x0000000000000000\t;; In script(long)  : %s\n",
		varEntry->generateName,
		varEntry->symbolName
		);
	return;
    }
    case SYMTYPE_FLOAT: {
	fprintf(_outputStream, "%s:\t\tdq\t0x0000000000000000\t;; In script(double): %s\n",
		varEntry->generateName,
		varEntry->symbolName
		);
	return;
    }
    case SYMTYPE_STRING: {
	fprintf(_outputStream, "%s:\t\tdq\tdaliys            \t;; In script(char*) : %s\n",
		varEntry->generateName,
		varEntry->symbolName
		);
	return;
    }
    }
    return;
}
void generateVarResList(miniTreeNode* _declarelist) {
    if (_declarelist == NULL) return;
    generateVarRes(_declarelist->nodeValue.def_list.def);
    generateVarResList(_declarelist->nodeValue.def_list.next);
    return;
}
void generateStrRes(miniTreeNode* _program) {
    assert(_program != NULL);
    putString("\"\"");
    strcpy(strManager->resID, "daliys");
    putDouble(0.0);
    strcpy(doubleManager->resID, "double_0");
    generateStrResStateList(_program->nodeValue.program.stateList);
    return;
}
void printSegData(miniTreeNode* _program) {
    fprintf(_outputStream, "segment .data\n");
    generateStrRes(_program);
    constStrListType* ptr = strManager;
    fprintf(_outputStream, "printfInt:   \tdb\t\"%%ld\",0xa,0x0\n");
    fprintf(_outputStream, "printfFloat: \tdb\t\"%%lf\",0xa,0x0\n");
    fprintf(_outputStream, "printfString:\tdb\t\"%%s\", 0xa,0x0\n");
    fprintf(_outputStream, "scanfInt:    \tdb\t\"%%ld\",0x0\n");
    fprintf(_outputStream, "scanfFloat:  \tdb\t\"%%lf\",0x0\n");
    while (ptr != NULL) {
	fprintf(_outputStream, "%s:\t\tdb\t%s,0x0\n",
		ptr->resID,
		ptr->strPtr
		);
	ptr = ptr->next;
    }
    constDoubleListType* ptr_double = doubleManager;
    while (ptr_double != NULL) {
	fprintf(_outputStream, "%s:\tdq\t__float64__(%lf)\n",
		ptr_double->resID,
		ptr_double->num
		);
	ptr_double = ptr_double->next;
    }
    generateVarResList(_program->nodeValue.program.defList);
    return;
}

void generateSegTextExpr(miniTreeNode* _expr) {
    assert(_expr != NULL);
    switch (_expr->nodeType) { 
    case INT_LITERIAL: {
	// Move the integer literal in rax, using 64-bits
	fprintf(_outputStream, "mov     \trax, 0x%016lx ;; load INT    literal : %ld\n",
		_expr->nodeValue.intLiterial,
		_expr->nodeValue.intLiterial);
	return;
    }
    case STRING_LITERIAL: {
	// Move the address of the stirng to rax, using 64-bits
	fprintf(_outputStream, "lea     \trax, [rel %s]        ;; load STRING literal : %s\n",
		getString(_expr->nodeValue.stringLiterial),
		_expr->nodeValue.stringLiterial);
	return;
    }
    case FLOAT_LITERIAL: {
	// Move the address of the double to xmm0, using 64-bits
	fprintf(_outputStream, "movsd   \txmm0, qword [rel %s] ;; load DOUBLE literal : %lf\n",
		getDouble(_expr->nodeValue.floatLiterial),
		_expr->nodeValue.floatLiterial);
	return;
    }
    case IDENTIFIER: {
	char* id = _expr->nodeValue.identifier;
	symLink* entry = getSymbol(_table, _expr);
	switch (entry->symbolType) {
	case SYMTYPE_INT:
	case SYMTYPE_STRING: {
	    fprintf(_outputStream, "mov     \trax, qword [rel %s]\t;; rax = %s\n",
		    entry->generateName,
		    entry->symbolName
		    );
	    return;
	}
	case SYMTYPE_FLOAT: {
	    fprintf(_outputStream, "movsd   \txmm0, qword [rel %s]\t;; xmm0 = %s\n",
		    entry->generateName,
		    entry->symbolName
		    );
	    return;
	}
	}
    }
    case EXPRESSION_PLUS: {
	miniTreeNode* left = _expr->nodeValue.exp_plus.left;
	miniTreeNode* right = _expr->nodeValue.exp_plus.right;
	typeCheckType leftType = oracleExpression(_table, left);
	typeCheckType rightType = oracleExpression(_table, right);
	switch (leftType) {
	case TYPECHECK_INT: {
	    switch (rightType) {
	    case TYPECHECK_INT: { // int + int
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rax, qword [rsp + 8] ", "");
		quickASM("add     ", "rax, [rsp]           ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case TYPECHECK_FLOAT: { // int + float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("addsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return; // Impossible
	    }
	    return;
	}
	case TYPECHECK_FLOAT: {
	    switch (rightType) {
	    case TYPECHECK_INT: { // float + int
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("addsd   ", "xmm0, [rsp]          ", "Restore 16-byte Stack");
		return;
	    }
	    case TYPECHECK_FLOAT: { // float + float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("addsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	    return;
	}
	case TYPECHECK_STRING: {
	    switch (rightType) {
	    case TYPECHECK_STRING: {
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rdi, qword [rsp + 8] ", "");
		quickASM("mov     ", "rsi, qword [rsp]     ", "");
		quickASM("xor     ", "rax, rax             ", "");
		quickASM("call    ", "_concatString        ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	    return;
	}
	default: return;
	}
    }
    case EXPRESSION_MINUS: {
	miniTreeNode* left = _expr->nodeValue.exp_minus.left;
	miniTreeNode* right = _expr->nodeValue.exp_minus.right;
	typeCheckType leftType = oracleExpression(_table, left);
	typeCheckType rightType = oracleExpression(_table, right);
	switch (leftType) {
	case TYPECHECK_INT: {
	    switch (rightType) {
	    case TYPECHECK_INT: { // int - int
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rax, qword [rsp + 8] ", "");
		quickASM("sub     ", "rax, qword [rsp]     ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case TYPECHECK_FLOAT: { // int - float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("subsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return; // Impossible
	    }
	    return;
	}
	case TYPECHECK_FLOAT: {
	    switch (rightType) {
	    case TYPECHECK_INT: { // float - int
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("subsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case TYPECHECK_FLOAT: { // float - float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("subsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	    return;
	}
	case TYPECHECK_STRING: {
	    switch (rightType) {
	    case TYPECHECK_STRING: {
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rdi, qword [rsp]     ", "");
		quickASM("xor     ", "rax, rax             ", "");
		quickASM("call    ", "_reverseString       ", "");
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rdi, qword [rsp + 8] ", "");
		quickASM("mov     ", "rsi, qword [rsp]     ", "");
		quickASM("xor     ", "rax, rax             ", "");
		quickASM("call    ", "_concatString        ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	    return;
	}
	default: return;
	}
    }
    case EXPRESSION_TIMES: {
	miniTreeNode* left  = _expr->nodeValue.exp_times.left;
	miniTreeNode* right = _expr->nodeValue.exp_times.right;
	typeCheckType leftType = oracleExpression(_table, left);
	typeCheckType rightType = oracleExpression(_table, right);
	switch (leftType) {
	case SYMTYPE_INT: { 
	    switch (rightType) { // int * int
	    case SYMTYPE_INT: {
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rax, qword [rsp + 8] ", "");
		quickASM("imul    ", "rax, qword [rsp]     ", "");
	       	quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case SYMTYPE_FLOAT: {// int * float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("mov     ", "rax, qword [rsp + 8] ", "");
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("mulsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	}
	case SYMTYPE_FLOAT: {
	    switch (rightType) {
	    case SYMTYPE_INT: {// float * int
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("mulsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case SYMTYPE_FLOAT: { // float * float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("mulsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	}
	default: return;
	}
    }
    case EXPRESSION_DIVIDE: {
	miniTreeNode* left  = _expr->nodeValue.exp_divide.left;
	miniTreeNode* right = _expr->nodeValue.exp_divide.right;
	typeCheckType leftType  = oracleExpression(_table, left);
	typeCheckType rightType = oracleExpression(_table, right);
	switch (leftType) {
	case SYMTYPE_INT: {
	    switch (rightType) {
	    case SYMTYPE_INT: { // int / int
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("mov     ", "qword [rsp + 8], rax ", "");
		generateSegTextExpr(right);
		quickASM("mov     ", "qword [rsp], rax     ", "");
		quickASM("mov     ", "rax, qword [rsp + 8] ", "");
		quickASM("xor     ", "rdx, rdx             ", "");
		quickASM("idiv    ", "qword [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case SYMTYPE_FLOAT: { // int / float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("divsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	}
	case SYMTYPE_FLOAT: {
	    switch (rightType) { // float / int
	    case SYMTYPE_INT: {
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("cvtsi2sd", "xmm0, rax            ", "");
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("divsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    case SYMTYPE_FLOAT: { // float / float
		quickASM("sub     ", "rsp, 16              ", "Reserve 16-byte Stack");
		generateSegTextExpr(left);
		quickASM("movsd   ", "qword [rsp + 8], xmm0", "");
		generateSegTextExpr(right);
		quickASM("movsd   ", "qword [rsp], xmm0    ", "");
		quickASM("movsd   ", "xmm0, qword [rsp + 8]", "");
		quickASM("divsd   ", "xmm0, [rsp]          ", "");
		quickASM("add     ", "rsp, 16              ", "Restore 16-byte Stack");
		return;
	    }
	    default: return;
	    }
	}
	default: return;
	}
    }
    case EXPRESSION_UNIARY_MINUS: {
	miniTreeNode* expr = _expr->nodeValue.exp_uniary_minus.expr;
	typeCheckType exprType = oracleExpression(_table, expr);
	switch (exprType) {
	case SYMTYPE_INT: {
	    generateSegTextExpr(expr);
	    quickASM("neg     ", "rax                      ", "rax = -rax");
	    return;
	}
	case SYMTYPE_FLOAT: {
	    quickASM("sub     ", "rsp, 8               ", "Reserve 8-byte Stack");
	    quickASM("mov     ", "qword [rsp], rcx     ", "");
	    quickASM("movq    ", "rax, xmm0            ", "");
	    quickASM("mov     ", "rcx, 0x8000000000000000", "");
	    quickASM("xor     ", "rax, rcx             ", "");
	    quickASM("movq    ", "xmm0, rax            ", "");
	    quickASM("mov     ", "rcx, qword [rsp]     ", "");
	    quickASM("add     ", "rsp, 8               ", "Restore 8-byte Stack");
	    return;
	}
        case SYMTYPE_STRING: {
	    quickASM("mov     ", "rdi, rax             ", "");
	    quickASM("call    ", "_reverseString       ", "");
	    return;
	}
	default: return;
	}
    }
    default: return;
    }
}

size_t labelCounter = 0L;
void generateSegTextStateList(miniTreeNode*);
void generateSegTextState(miniTreeNode* _state) {
    assert(_state != NULL);
    switch (_state->nodeType) {
    case STATEMENT_ASSIGN: {
	typeCheckType type = oracleExpression(_table, _state->nodeValue.state_assign.expr);
	symLink* symEntry  = getSymbol(_table, _state->nodeValue.state_assign.id);
	char*    id        = symEntry->generateName;
	generateSegTextExpr(_state->nodeValue.state_assign.expr);
	switch (symEntry->symbolType) {
	case SYMTYPE_STRING:
	case SYMTYPE_INT: {
	    fprintf(_outputStream,"mov     \tqword [rel %s], rax  \t;; %s = rax\n",
		    id, symEntry->symbolName);
	    return;
	}
	case SYMTYPE_FLOAT: {
	    fprintf(_outputStream,"movsd   \tqword [rel %s], xmm0 \t;; %s = xmm0\n",
		    id, symEntry->symbolName);
	    return;
	}
	}
    }
    case STATEMENT_PRINT: {
	generateSegTextExpr(_state->nodeValue.state_print.expr);
	typeCheckType exprType = oracleExpression(_table, _state->nodeValue.state_print.expr);
	switch (exprType) {
	case TYPECHECK_INT: {
	    quickASM("lea     ", "rdi, [rel printfInt] ", "");
	    quickASM("mov     ", "rsi, rax             ", "");
	    quickASM("xor     ", "rax, rax             ", "");
	    quickASM("call    ", "_printf              ", "invoke printf int");
	    return;
	}
	case TYPECHECK_FLOAT: {
	    quickASM("lea     ", "rdi, [rel printfFloat]", "");
	    quickASM("mov     ", "rax, 0x0000000000000001","");
	    quickASM("call    ", "_printf              ", "invoke printf float");
	    return;
	}
	case TYPECHECK_STRING: {
	    quickASM("lea     ", "rdi, [rel printfString]","");
	    quickASM("mov     ", "rsi, rax             ", "");
	    quickASM("xor     ", "rax, rax             ", "");
	    quickASM("call    ", "_printf              ", "invoke printf string");
	    return;
	}
	default: return;
	}
	return;
    }
    case STATEMENT_READ: {
	miniTreeNode* identifier = _state->nodeValue.state_read.expr;
	symLink* type = getSymbol(_table, identifier);
	char*    generateLable = type->generateName;
	switch (type->symbolType) {
	case SYMTYPE_INT: {
	    quickASM("lea     ", "rdi, [rel scanfInt]  ", "");
	    fprintf(_outputStream,"lea     \trsi, [rel %s]        \t;; \n",
		    generateLable);
	    quickASM("call    ", "_scanf               ", "");
	    return;
	}
	case SYMTYPE_FLOAT: {
	    quickASM("lea     ", "rdi, [rel scanfFloat]", "");
	    fprintf(_outputStream,"lea     \trsi, [rel %s]        \t;; \n",
		    generateLable);
	    quickASM("call    ", "_scanf               ", "");
	    return;
	}
	case SYMTYPE_STRING: {
	    quickASM("call    ", "_scanString          ", "");
	    fprintf(_outputStream,"mov     \tqword [rel %s], rax  \t;; \n",
		    generateLable);
	    return;
	}
	}
	return;
    }
    case STATEMENT_IF: {
	char* labelifEnd   = (char*)malloc(sizeof(char)*256);
	sprintf(labelifEnd,   "LL%04zu", labelCounter++);
	generateSegTextExpr(_state->nodeValue.state_if.expr);
	typeCheckType type = oracleExpression(_table, _state->nodeValue.state_if.expr);
        assert(type == TYPECHECK_INT);
	quickASM("cmp     ", "rax, 0x000000000000000", "");
	fprintf(_outputStream,"jz      \t%s                 \t;; \n", labelifEnd);
	generateSegTextStateList(_state->nodeValue.state_if.state);
	fprintf(_outputStream, "%s:\n", labelifEnd);
	free(labelifEnd);
	return;
    }
    case STATEMENT_IF_ELSE: {
	char* labelifFalse = (char*)malloc(sizeof(char)*256);
	char* labelifEnd   = (char*)malloc(sizeof(char)*256);
	sprintf(labelifFalse, "LL%04zu", labelCounter++);
	sprintf(labelifEnd, "LL%04zu", labelCounter++);
	generateSegTextExpr(_state->nodeValue.state_if_else.expr);
	typeCheckType type = oracleExpression(_table, _state->nodeValue.state_if_else.expr);
        assert(type == TYPECHECK_INT);
	quickASM("cmp     ", "rax, 0x000000000000000", "");
	fprintf(_outputStream,"jz      \t%s                 \t;; \n", labelifFalse);
	generateSegTextStateList(_state->nodeValue.state_if_else.state_true);
	fprintf(_outputStream,"jmp     \t%s                 \t;; \n", labelifEnd);
	fprintf(_outputStream, "%s:\n", labelifFalse);
	generateSegTextStateList(_state->nodeValue.state_if_else.state_false);
	fprintf(_outputStream, "%s:\n", labelifEnd);
	free(labelifFalse);
	free(labelifEnd);
	return;
    }
    case STATAEMENT_WHILE: {
	char* labelwhileCheck = (char*)malloc(sizeof(char)*256);
	char* labelwhileEnd   = (char*)malloc(sizeof(char)*256);
	sprintf(labelwhileCheck, "LL%04zu", labelCounter++);
	sprintf(labelwhileEnd, "LL%04zu", labelCounter++);
	fprintf(_outputStream, "%s:\n", labelwhileCheck);
	generateSegTextExpr(_state->nodeValue.state_while.expr);
	typeCheckType type = oracleExpression(_table, _state->nodeValue.state_while.expr);
	assert(type == TYPECHECK_INT);
	quickASM("cmp     ", "rax, 0x000000000000000", "");
	fprintf(_outputStream,"jz      \t%s                 \t;; \n", labelwhileEnd);
	generateSegTextStateList(_state->nodeValue.state_while.state);
	fprintf(_outputStream,"jmp     \t%s                 \t;; \n", labelwhileCheck);
	fprintf(_outputStream, "%s:\n", labelwhileEnd);
	free(labelwhileCheck);
	free(labelwhileEnd);
	return;
    }
    default: return;
    }
    return;
}
void generateSegTextStateList(miniTreeNode* _list){
    if (_list == NULL) return;
    generateSegTextState    (_list->nodeValue.state_list.expr);
    generateSegTextStateList(_list->nodeValue.state_list.next);
    return;
}
void printSegText(miniTreeNode* _list){
    fprintf(_outputStream, "segment .text\n");
    fprintf(_outputStream, "extern _concatString       ;; addrType concatString(addrType, addrType) (miniLang library)\n");
    fprintf(_outputStream, "extern _reverseString      ;; addrType reverseString(addrType)          (miniLang library)\n");
    fprintf(_outputStream, "extern _scanString         ;; addrType scanString()                     (miniLang library)\n");
    fprintf(_outputStream, "extern _initMemoryPool     ;; void     initMemoryPool()                 (miniLang library)\n");
    fprintf(_outputStream, "extern _exitMemoryPool     ;; void     exitMemroyPool()                 (miniLang library)\n");
    fprintf(_outputStream, "extern _printf             ;; int      printf(const char *restrict,...) (C library)\n");
    fprintf(_outputStream, "extern _scanf              ;; int      scanf(const char *restrict,...)  (C library)\n");
    
    fprintf(_outputStream, "global _main\n");
    fprintf(_outputStream, "_main:\n");
    quickASM("push    ", "rbp                  ", "Init Stack Frame");
    quickASM("mov     ", "rbp, rsp             ", "");
    quickASM("call    ", "_initMemoryPool      ", "Init Memory Pool");
    fprintf(_outputStream, "\n");
    
    generateSegTextStateList(_list->nodeValue.program.stateList);
    
    fprintf(_outputStream, "\n");
    quickASM("call    ", "_exitMemoryPool      ", "Exit Memroy Pool");
    quickASM("mov     ", "rsp, rbp             ", "");
    quickASM("pop     ", "rbp                  ", "Exit Stack Frame");
    fprintf(_outputStream, "\n");
    quickASM("mov     ", "rax, 0x0000000002000001", "Mac OS X exit syscall code");
    quickASM("mov     ", "rdi, 0x0             ", "Normal Exit (exit code = 0)");
    quickASM("syscall ", "                     ", "Init Syscall");
    return;
}
#endif

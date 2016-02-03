#ifndef __MINI__PRETTYPRINT__H__
#define __MINI__PRETTYPRINT__H__

#include "treeNode.h"
#include <stdlib.h>
#include <stdio.h>

size_t _indentLevel = 0L;
extern FILE* _outputStream;

void prettyPrint(miniTreeNode* _node){
	if (_node == NULL) return;
	switch (_node->nodeType) {
		case TYPE_INT    : {fprintf(_outputStream,"int"); return;}
		case TYPE_FLOAT  : {fprintf(_outputStream,"float"); return;}
		case TYPE_STRING : {fprintf(_outputStream,"string"); return;}
		case INT_LITERIAL: {
			fprintf(_outputStream,"%ld", _node->nodeValue.intLiterial); 
			return;
		}
		case STRING_LITERIAL : {
			fprintf(_outputStream,"%s", _node->nodeValue.stringLiterial);
			return;
		}
		case FLOAT_LITERIAL : {
			fprintf(_outputStream,"%lf", _node->nodeValue.floatLiterial);
			return;
		}
		case IDENTIFIER : {
			fprintf(_outputStream,"%s", _node->nodeValue.identifier);
			return;
		}
		case DECLARATION : {
			fprintf(_outputStream,"var ");
			prettyPrint(_node->nodeValue.declaration.id);
			fprintf(_outputStream," : ");
			prettyPrint(_node->nodeValue.declaration.type);
			fprintf(_outputStream,"; \n");
			return;
		}
		case EXPRESSION_PLUS : {
			fprintf(_outputStream,"(");
			prettyPrint(_node->nodeValue.exp_plus.left);
			fprintf(_outputStream,"+");
			prettyPrint(_node->nodeValue.exp_plus.right);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_MINUS : {
			fprintf(_outputStream,"(");
			prettyPrint(_node->nodeValue.exp_minus.left);
			fprintf(_outputStream,"-");
			prettyPrint(_node->nodeValue.exp_minus.right);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_TIMES : {
			fprintf(_outputStream,"(");
			prettyPrint(_node->nodeValue.exp_times.left);
			fprintf(_outputStream,"*");
			prettyPrint(_node->nodeValue.exp_times.right);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_DIVIDE : {
			fprintf(_outputStream,"(");
			prettyPrint(_node->nodeValue.exp_divide.left);
			fprintf(_outputStream,"/");
			prettyPrint(_node->nodeValue.exp_divide.right);
			fprintf(_outputStream,")");
			return;
		}
		case EXPRESSION_UNIARY_MINUS : {
			fprintf(_outputStream,"-(");
			prettyPrint(_node->nodeValue.exp_uniary_minus.expr);
			fprintf(_outputStream,")");
			return;
		}
		case STATEMENT_ASSIGN : {
			prettyPrint(_node->nodeValue.state_assign.id);
			fprintf(_outputStream," = ");
			prettyPrint(_node->nodeValue.state_assign.expr);
			fprintf(_outputStream,";\n");
			return;
		}
		case STATEMENT_PRINT : {
			fprintf(_outputStream,"print ");
			prettyPrint(_node->nodeValue.state_print.expr);
			fprintf(_outputStream,";\n");
			return;
		}
		case STATEMENT_READ : {
			fprintf(_outputStream,"read ");
			prettyPrint(_node->nodeValue.state_read.expr);
			fprintf(_outputStream,";\n");
			return;
		}
		case STATEMENT_IF : {
			fprintf(_outputStream,"if ");
			prettyPrint(_node->nodeValue.state_if.expr);
			fprintf(_outputStream," then\n");
			_indentLevel++;
			prettyPrint(_node->nodeValue.state_if.state);
			_indentLevel--;
			fprintf(_outputStream,"endif\n");
			return;
		}
		case STATEMENT_IF_ELSE : {
			fprintf(_outputStream,"if ");
			prettyPrint(_node->nodeValue.state_if_else.expr);
			fprintf(_outputStream," then\n");
			_indentLevel++;
			prettyPrint(_node->nodeValue.state_if_else.state_true);
			_indentLevel--;
			for (size_t iter = 0; iter < _indentLevel; iter++)
				fprintf(_outputStream, "\t");
			fprintf(_outputStream,"else\n");
			_indentLevel++;
			prettyPrint(_node->nodeValue.state_if_else.state_false);
			_indentLevel--;
			for (size_t iter = 0; iter < _indentLevel; iter++)
				fprintf(_outputStream,"\t");
			fprintf(_outputStream,"endif\n");
			return;
		}
		case STATAEMENT_WHILE : {
			fprintf(_outputStream,"while ");
			prettyPrint(_node->nodeValue.state_while.expr);
			fprintf(_outputStream," do\n");
			_indentLevel++;
			prettyPrint(_node->nodeValue.state_while.state);
			_indentLevel--;
			for (size_t iter = 0; iter < _indentLevel; iter++)
				fprintf(_outputStream,"\t");
			fprintf(_outputStream,"done\n");
			return;
		}
		case STATEMENT_LIST : {
			for (size_t iter = 0; iter < _indentLevel; iter++)
				fprintf(_outputStream,"\t");
			prettyPrint(_node->nodeValue.state_list.expr);
			prettyPrint(_node->nodeValue.state_list.next);
			return;
		}
		case DECLARATION_LIST : {
			prettyPrint(_node->nodeValue.def_list.def);
			prettyPrint(_node->nodeValue.def_list.next);
			return;
		}
		case MINI_PROGRAM : {
			prettyPrint(_node->nodeValue.program.defList);
			fprintf(_outputStream,"\n");
			prettyPrint(_node->nodeValue.program.stateList);
			return;
		}
		default : return;
	}
}

#endif
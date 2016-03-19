#ifndef __MINI__MAINFUNC__H__
#define __MINI__MAINFUNC__H__

#include <string.h>

bool	_dryRun  = false;
bool	_emitSymbol = false;
bool 	_emitPretty = false;
bool 	_flag		= false;
extern const char* _filename;
extern FILE* _outputStream;
extern memoryList _memlist;
extern symbolTable _table;
char* _outputTarget = NULL;

void cleanup(){
	if (_outputStream != stdout) fclose(_outputStream);
	if (_memlist != NULL)		 freeList(_memlist);
	return;
}

void throwError(const char* _what){		//Will be invoked as program failed
	fprintf(stderr, "\e[31;1m[Error]\e[0m %s", _what);
	#ifdef PRINTSYMTABLE_ON_EXIT
	if (_outputStream != stdout) fclose(_outputStream);
	if (_emitSymbol){
		char* filePath = (char*)malloc(sizeof(char)*1024);
		if (_outputTarget != NULL){
			sprintf(filePath, "%s.symbol.txt", _outputTarget);
			_outputStream = fopen(filePath, "w");
			free(filePath);
		}
		printSymbolTable(_table);
		if (_outputStream != stdout){
			fclose(_outputStream);
			_outputStream = stdout;
		}
	}
	#endif
	cleanup();
	exit(1);
}

#define ARGV_USAGE	"-v"
#define ARGV_INPUT  "-i"
#define ARGV_OUTPUT	"-o"
#define ARGV_EMIT_SYMBOLTABLE "-S"   //xxx.symbol.txt
#define ARGV_EMIT_PRETTYPRINT "-P"   //xxx.pretty.min

bool checkExistance(const char * filename){
	FILE* file = fopen(filename, "r");
    if (file!=NULL) {
        fclose(file);
        return true;
    }
    return false;
}
void printUsage(){
#ifdef ASM
	fprintf(stdout,"MINILANG Assembly(Intel Syntax) Compiler\n");
#else
	fprintf(stdout,"MINILANG C Compiler\n");
#endif
	fprintf(stdout,"Usage : ./mini [-S] [-P] -i inputPath [-o targetName]\n");
	fprintf(stdout,"\t-S\temit symbol table\n");
	fprintf(stdout,"\t-P\temit pretty print\n");
	fprintf(stdout,"\t-i\tredirect stdin to file inputPath\n");
	fprintf(stdout,"\t-o\tredirect stdout\n");
	fprintf(stdout,"\t  \t\tprettyPrint File : $(outputPath).pretty.min\n");
	fprintf(stdout,"\t  \t\tsymbolTable File : $(outputPath).symbol.txt\n");
#ifdef ASM
	fprintf(stdout,"\t  \t\tAssembly Code    : $(outputPath).s\n");
#else
	fprintf(stdout,"\t  \t\tC Code           : $(outputPath).c\n");
#endif
	cleanup();
	exit(0);
	return;
}
void estimateOuputPath(){
	long pos = 0L;
	for (long iter = (long)strlen(_filename)-1; iter >= 0L; iter--) {
		if (_filename[iter]=='.') {
			pos = iter;
			break;
		}
	}
	_outputTarget = (char*)mallocList(sizeof(char)*(strlen(_filename)+1), _memlist);
	memset(_outputTarget, '\0', sizeof(char)*(strlen(_filename)+1));
	strncpy(_outputTarget, _filename, pos);
	return;
}
int main(int argc, char const *argv[]) {
	_outputStream = stdout;
	_memlist = newMemoryList();
	_program = NULL;
	for (int iter = 1; iter < argc; iter++){
		if (strcmp(argv[iter],ARGV_USAGE)==0){
			_dryRun = true;
			continue;
		}
		if (strcmp(argv[iter],ARGV_EMIT_SYMBOLTABLE)==0){
			_emitSymbol = true;
			continue;
		}
		if (strcmp(argv[iter],ARGV_EMIT_PRETTYPRINT)==0){
			_emitPretty = true;
			continue;
		}
		if (strcmp(argv[iter],ARGV_INPUT)==0){
			if (iter + 1 >= argc) {_dryRun = true; break;}
			if (checkExistance(argv[iter+1]) == false) {_dryRun = true; break;}
			freopen(argv[iter+1],"r",stdin);
			_filename = argv[iter+1];
			_flag = true;
			continue;
		}
		if (strcmp(argv[iter],ARGV_OUTPUT) == 0){
			if (iter + 1 >= argc) {_dryRun = true; break;}
			_outputTarget = (char*)argv[iter+1];
			continue;
		}
	}
	if (_flag == false) _dryRun = true;
	if (_dryRun == true) printUsage();
	if (_outputTarget == NULL) estimateOuputPath();
	
	// Parsing MINI LANG
	yyparse();
	if (_program == NULL) {
		cleanup();
		exit(1);
	}
	
	// Pretty Print MINI LANG
	if (_emitPretty){
		char* filePath = (char*)malloc(sizeof(char)*1024);
		if (_outputTarget != NULL){
			sprintf(filePath, "%s.pretty.min", _outputTarget);
			_outputStream = fopen(filePath, "w");
			free(filePath);
		}
		prettyPrint(_program);
		if (_outputStream != stdout){
			fclose(_outputStream);
			_outputStream = stdout;
		}
	}
	
	// Generating Symbol Table
	_table = newSymbolTable();
	generateSymbolTable(_table, _program);
	if (_emitSymbol){
		char* filePath = (char*)malloc(sizeof(char)*1024);
		if (_outputTarget != NULL){
			sprintf(filePath, "%s.symbol.txt", _outputTarget);
			_outputStream = fopen(filePath, "w");
			free(filePath);
		}
		printSymbolTable(_table);
		if (_outputStream != stdout){
			fclose(_outputStream);
			_outputStream = stdout;
		}
	}
	scanSymbolTable(_table);
	
	// Type checking
	oracleProgram(_table, _program);
	
	// Code Generation
	char* filePath = (char*)malloc(sizeof(char)*1024);
	if (_outputTarget != NULL){
#ifdef ASM
		sprintf(filePath, "%s.s", _outputTarget);
#else
		sprintf(filePath, "%s.c", _outputTarget);
#endif
		_outputStream = fopen(filePath, "w");
		free(filePath);
	}
#ifdef ASM
	printSegData(_program);
	printSegText(_program);
#else
	generate(_program);
#endif
	if (_outputStream != stdout){
		fclose(_outputStream);
		_outputStream = stdout;
	}
	
	cleanup();
    return 0;
}

#endif
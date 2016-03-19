LEX = ./src/mini.l
YACC = ./src/mini.y
OBJ = mini
CC = clang -DDEBUG -DASM
FLEX = flex
BISON = bison
H = ./src/codeGenerate.h ./src/codeGenerateASM.h ./src/mainFunc.c ./src/memory.h ./src/prettyPrint.h ./src/symTable.h ./src/treeNode.h ./src/typeChecking.h
    
all    : $(OBJ) lib

$(OBJ) : $(LEX) $(YACC) $(H)
	$(FLEX) $(LEX)
	$(BISON) $(YACC)
	mv mini.tab.c parser.h
	$(CC) -std=c99 -I./src/ -g -DASM lex.yy.c -o $(OBJ) -ll
	rm lex.yy.c parser.h

lib    : ./src/miniLang.c
	clang -c ./src/miniLang.c
	ar -cvq libminiLang.a miniLang.o
	rm miniLang.o

clean :
	rm -f mini libminiLang.a
	rm -r -f mini.dSYM
	rm -f *.min
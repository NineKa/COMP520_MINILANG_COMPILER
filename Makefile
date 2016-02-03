LEX = ./src/mini.l
YACC = ./src/mini.y
OBJ = mini
CC = gcc
FLEX = flex
BISON = bison
    
$(OBJ) : $(LEX) $(YACC)
	$(FLEX) $(LEX)
	$(BISON) $(YACC)
	mv mini.tab.c parser.h
	$(CC) -std=c99 -I./src/ -g -DPRINTSYMTABLE_ON_EXIT -DDEBUG lex.yy.c -o $(OBJ) -lfl

clean :
	rm mini lex.yy.c parser.h
	rm -r -f mini.dSYM
	rm *.min
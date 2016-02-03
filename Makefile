LEX = ./src/mini.l
YACC = ./src/mini.y
OBJ = mini
CC = clang 
FLEX = /usr/local/Cellar/flex/2.6.0/bin/flex
BISON = /usr/local/Cellar/bison/3.0.4/bin/bison
    
$(OBJ) : $(LEX) $(YACC)
	$(FLEX) $(LEX)
	$(BISON) $(YACC)
	mv mini.tab.c parser.h
	$(CC) lex.yy.c -I./src/ -g -DPRINTSYMTABLE_ON_EXIT -DDEBUG -o $(OBJ) -ll

clean :
	rm mini lex.yy.c parser.h
	rm -r -f mini.dSYM
	rm *.min
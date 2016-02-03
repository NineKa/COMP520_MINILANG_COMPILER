Build:
$make

C codes are generated in C99 standard, compile with:
$(CC) -std=c99 $(src) -o $(obj)

MINILANG C Compiler
Usage : ./mini [-S] [-P] -i inputPath [-o targetName]
	-S	emit symbol table
	-P	emit pretty print
	-i	redirect stdin to file inputPath
	-o	redirect stdout
	  		prettyPrint File : $(outputPath).pretty.min
	  		symbolTable File : $(outputPath).symbol.txt
	  		C Code           : $(outputPath).c
			
:D

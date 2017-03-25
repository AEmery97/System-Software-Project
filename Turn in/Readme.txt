To compile:
	gcc Compiler.c Parser.c Scanner.c pmachine.c
To run:
	./a.out [options] [file]
	
	Options:
		-l : print the list of lexemes/tokens (scanner output) to the screen

		-a : print the generated assembly code (parser/codegen output) to the screen

		-v : print virtual machine execution trace (virtual machine output) to the screen

Generated Assembly code is placed in a file named assembly.txt

Code size, maximum number of tokens, and other options are defined in Compiler.h

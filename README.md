# lab1DSITJZ
Lab 1: Scripting Language with Neil Roy, Dalia Sebat, Ilai Tamari, Javin Zipkin

This program uses a lexer and a parser to interpret a basic scripting language. It supports variables, branches, loops, functions (with closures), and arrays (with builtins.)

src/lex.cpp: Main function that tests the lexer.   
src/calc.cpp: Main function that tests the lexer and the infix parser together.   
src/format.cpp: Main function that formats a program, testing the blocking process.
src/scrypt.cpp: Main function that interprets a basic program.

src/Makefile: Compiles with flags automatically.    
    To run the calculator:   
    make calc   
    ./calc    
    To automatically format your program:
    make format
    ./format
    To run an entire program:
    make scrypt
    ./scrypt
src/withText.cpp: Runs scrypt or format on a text file (to use, name it input.txt and write whatever you want) rather than standard input for convenience.
To use this specifically:
write your text file in src and call it input.txt
make withText
./withText 0 (for SCRYPT)
./withText 1 (for FORMAT)

lib/token.h: Header file for the Token struct, which stores text, line, and column information to be stored in the lexer and parser. Users need not interact with this struct directly.
lib/typedValue.h: Header file for the TypedValue struct, which is passed around by the Parser's functions as a way to store different types of data in variables and expressions. Users need not interact with this class directly.
lib/block.h: Header file for the Block struct, which stores information about statements and the expressions nested within them. Supports expressions, if, else, else/if, while, and print. Users need not interact with this struct directly.
lib/node.h: Header file for the Node struct, which supports AST nodes for the parser. Users need not interact with this struct directly.
lib/lex.h: Header file for the lexer.   
lib/lexer.cpp: Implements all lexer functions, including constructing and printing a collection of Tokens.   
lib/parser.h: Header file for the infix parser.   
lib/parser.cpp: Implements all parser functions, including constructing, printing, evaluating, and clearing the parser's AST for infix expressions, as well as formatting and interpreting full programs.    
lib/parserUtil.cpp: Implements less substantial parser functionality.


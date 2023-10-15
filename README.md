# lab1DSITJZ
Lab 1: Calculator with Neil Roy, Dalia Sebat, Ilai Tamari, Javin Zipkin

This program uses a lexer and a parser to evaluate algebraic S-expressions from standard input and will catch
syntax rrors and invalid expressions.

src/lex.cpp: Main function that tests the lexer
src/parse.cpp: Main function that tests the lexer and the parser together
lib/token.h: Header file for the Token struct, which stores text, line, and column information to be stored in the lexer and parser.
Users should not have to interact with the Token struct at all.
lib/lex.h: Header file for the lexer
lib/lex.cpp: Implements all lexer functions, including constructing and printing a collection of Tokens
lib/parser.h: Header file for the parser
lib/parser.cpp: Implements all parser functions, including constructing, printing, evaluating, and clearing the parser's AST
src/Makefile: Compiles stuff with flags automatically
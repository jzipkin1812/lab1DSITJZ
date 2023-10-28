lex: lex.cpp lib/lexer.cpp parse.cpp lib/parse.h lib/lex.h lib/token.h 
	g++ -o lex -std=c++17 -Wall -Wextra -Werror lex.cpp lib/lexer.cpp

calc: calc.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/node.h lib/block.h
	g++ -o calc -std=c++17 -Wall -Wextra -Werror calc.cpp lib/parser.cpp lib/lexer.cpp
format: format.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp
	g++ -o format -std=c++17 -Wall -Wextra -Werror format.cpp lib/parser.cpp lib/lexer.cpp
parse: parse.cpp lib/oldParser.h lib/token.h lib/oldParser.cpp
	g++ -o parse -std=c++17 -Wall -Wextra -Werror parse.cpp lib/oldParser.cpp lib/lexer.cpp 
leak: calc calc.cpp lib/parser.cpp
	valgrind --leak-check=full ./calc
clean:
	rm -f project
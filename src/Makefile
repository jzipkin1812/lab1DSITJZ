lex: lex.cpp lib/lexer.cpp lib/parse.h lib/lex.h lib/token.h 
	g++ -o lex -std=c++17 -Wall -Wextra -Werror lex.cpp lib/lexer.cpp
calc: calc.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/node.h lib/block.h
	g++ -o calc -std=c++17 -Wall -Wextra -Werror calc.cpp lib/parser.cpp lib/lexer.cpp
format: format.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/block.h
	g++ -o format -std=c++17 -Wall -Wextra -Werror format.cpp lib/parser.cpp lib/lexer.cpp
scrypt: scrypt.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/block.h
	g++ -o scrypt -std=c++17 -Wall -Wextra -Werror scrypt.cpp lib/parser.cpp lib/lexer.cpp
leak: calc calc.cpp lib/parser.cpp
	valgrind --leak-check=full ./calc
clean:
	rm -f project
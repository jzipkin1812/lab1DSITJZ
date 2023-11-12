lex: lex.cpp lib/lexer.cpp lib/parse.h lib/lex.h lib/parserUtil.cpp lib/token.h 
	g++ -o lex -std=c++17 -Wall -Wextra -Werror lex.cpp lib/lexer.cpp
calc: calc.cpp lib/typedValue.h lib/parse.h lib/parserUtil.cpp lib/token.h lib/lexer.cpp lib/parser.cpp lib/node.h lib/block.h
	g++ -o calc -std=c++17 -Wall -Wextra -Werror calc.cpp lib/parser.cpp lib/lexer.cpp lib/parserUtil.cpp
format: format.cpp lib/typedValue.h lib/parserUtil.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/block.h
	g++ -o format -std=c++17 -Wall -Wextra -Werror format.cpp lib/parser.cpp lib/lexer.cpp lib/parserUtil.cpp
scrypt: scrypt.cpp lib/typedValue.h lib/parserUtil.cpp lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/block.h
	g++ -o scrypt -std=c++17 -Wall -Wextra -Werror scrypt.cpp lib/parser.cpp lib/lexer.cpp lib/parserUtil.cpp
withText: lib/node.h input.txt lib/parserUtil.cpp withText.cpp lib/typedValue.h lib/parse.h lib/token.h lib/lexer.cpp lib/parser.cpp lib/block.h
	g++ -o withText -std=c++17 -Wall -Wextra -Werror withText.cpp lib/parser.cpp lib/lexer.cpp lib/parserUtil.cpp
leak: withText lib/parser.cpp
	valgrind --leak-check=full ./withText 0
clean:
	rm -f project
	rm -f scrypt
	rm -f calc
	rm -f format
	rm -f withText
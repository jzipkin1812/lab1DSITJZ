lex: lex.cpp lib/lexer.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h 
	g++ -o lex -std=c++17 -Wall -Wextra -Werror lex.cpp lib/lexer.cpp

calc: calc.cpp lib/parse.h lib/token.h 
	g++ -o calc -std=c++17 -Wall -Wextra -Werror calc.cpp lib/parser.cpp lib/lexer.cpp

clean:
	rm -f project
	rm calc
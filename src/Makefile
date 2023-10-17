lex: lex.cpp lib/lexer.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h lib/parser.cpp lib/lexer.cpp
	g++ -o lex -std=c++17 -Wall -Wextra -Werror lex.cpp lib/lexer.cpp
parse: lex.cpp lib/lexer.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h lib/parser.cpp lib/lexer.cpp
	g++ -o parse -std=c++17 -Wall -Wextra -Werror parse.cpp lib/lexer.cpp lib/parser.cpp
clean:
	rm -f project
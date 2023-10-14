test: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	g++ -o test -std=c++17 -Wall -Wextra -Werror test.cpp lex.cpp parse.cpp 
parse: lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h lib/parser.cpp
	g++ -o parse -std=c++17 -Wall -Wextra -Werror parse.cpp lib/parser.cpp lib/lexer.cpp
leak: lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	valgrind --leak-check=full --show-leak-kinds=all ./parse 
clean:
	rm -f test

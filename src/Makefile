test: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	g++ -o test -std=c++17 -Wall -Wextra -Werror test.cpp lex.cpp parse.cpp 
parse: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	g++ -o parse -std=c++17 -Wall -Wextra -Werror parse.cpp lex.cpp
clean:
	rm -f test

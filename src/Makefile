test: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	g++ -o test -std=c++17 -Wall -Wextra -Werror test.cpp lex.cpp parse.cpp 
parse: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	g++ -o parse -std=c++17 -Wall -Wextra -Werror parse.cpp lib/lexFunctions.cpp
lex: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	g++ -o lex -std=c++17 -Wall -Wextra -Werror lex.cpp 
leak: test.cpp lex.cpp parse.cpp lib/parser.h lib/lex.h lib/token.h
	valgrind --leak-check=full --show-leak-kinds=all ./parse 
clean:
	rm -f test

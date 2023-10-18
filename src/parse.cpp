#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "lib/parser.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;
int main()
{
    Lexer myLexer = Lexer();
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print();
}

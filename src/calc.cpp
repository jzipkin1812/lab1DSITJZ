#include <iostream>
#include <string>
#include <vector>
#include "lib/parse.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;

int main()
{
    Lexer myLexer = Lexer(true);
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print(); // calls evaluate() internally.
    return 0;
}
#include <iostream>
#include <string>
#include <vector>
#include "lib/parse.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;

int main()
{
    Lexer myLexer = Lexer(true, true);
    // myLexer.print();
    Parser myParser = Parser(myLexer.getTokens(), true);
    //myParser.format();
    myParser.execute(); // Calls evaluate. Only prints when prompted.
    return 0;
}
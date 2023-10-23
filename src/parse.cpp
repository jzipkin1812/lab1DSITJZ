#include <iostream>
#include <string>
#include "lib/oldParser.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;
int main()
{
    Lexer myLexer = Lexer();
    OldParser myParser = OldParser(myLexer.getTokens());
    myParser.print();
}

#include <iostream>
#include <string>
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;
int main()
{
    Lexer myLexer = Lexer();
    OldParser myParser = OldParser(myLexer.getTokens()); //oldParser is original S-expression parser from Checkpoint 1
    myParser.print();
}

#include <iostream>
#include <string>
#include "lib/parser.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;
int main()
{
    // cout << sExpression << endl;
    Lexer myLexer = Lexer();
    // for(Token t : myLexer.getTokens())
    // {
    //     cout << t.text << " ";
    // }
    // cout << endl;
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print();
}
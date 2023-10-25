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
    //Parser myParser = Parser(myLexer.getTokens());
    //myParser.print(); // calls evaluate() internally.
    vector<Token> tokens = myLexer.getTokens()[0];
    for (Token token : tokens) cout << token.text << " ";
    cout << endl;
    return 0;
}
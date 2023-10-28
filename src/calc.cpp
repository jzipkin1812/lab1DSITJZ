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
    for(vector<Token> line : myLexer.getTokens())
    {
        for(Token t : line)
        {
            cout << t.text << " ";
        }
        cout << endl;
    }
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print(); // calls evaluate() internally.
    return 0;
}
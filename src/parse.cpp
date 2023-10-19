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
    for(vector<Token> line : myLexer.getTokens())
    {
        for(Token t : line)
        {
            cout << t.text << " ";
        }
        cout << endl;
    }
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print();
}

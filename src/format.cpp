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
    // for(vector<Token> line : myLexer.getTokens())
    // {
    //     for(Token t : line)
    //     {
    //         cout << t.text << " ";
    //     }
    //     cout << endl;
    // }
    Parser myParser = Parser(myLexer.getTokens(), true);
    myParser.format(); // DOES NOT CALL EVALUATE(). Prints the program raw (with parentheses.)
    return 0;
}
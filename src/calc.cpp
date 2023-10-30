#include <iostream>
#include <string>
#include <vector>
#include "lib/parse.h"
#include "lib/token.h"
#include "lib/lex.h"
#include "lib/typedValue.h"


using namespace std;

int main()
{
    Lexer myLexer = Lexer(true);
    // for(vector<Token> line : myLexer.getTokens())
    // {
    //     for(Token t : line)
    //     {
    //         cout << t.text << " ";
    //     }
    //     cout << endl;
    // }
    // Token t = Token{1, 1, "true"};
    // Token e1 = Token{1, 2, "END"};
    // Token e2 = Token{1, 3, "END"};
    // vector<vector<Token>> testEof;
    // vector<Token> three = {t, e1, e2};
    // testEof.push_back(three);
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print(); // calls evaluate() internally.
    return 0;
}
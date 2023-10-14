#include <iostream>
#include <vector>
#include <iomanip>
#include <ctype.h>
#include "lib/lex.h"
#include "lib/parser.h"
#include "lib/token.h"
using namespace std;

int main()
{
    Lexer myLexer = Lexer();
    myLexer.print();
    // Parser myParser = Parser(myLexer.getTokens());
    // myParser.print();
    // cout << "Expression evaluates to: " << myParser.evaluate();

    return (0);
}

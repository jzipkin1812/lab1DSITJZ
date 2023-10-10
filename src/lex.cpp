#include <iostream>
#include <vector>
#include "lib/lex.h"
#include "lib/parser.h"
#include "lib/token.h"
using namespace std;

Lexer::vector<Token> tokenize(string expression)
{
    vector<Token> allTokens;
    return(allTokens); //CODE STUB
}

int main()
{
    string sExpression;
    
    cout << "Input the S-expression: ";
    cin >> sExpression;

    Lexer myLexer = Lexer(sExpression);
    myLexer.print();
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print();
    cout << "Expression evaluates to: " << myParser.evaluate();

    return(0);
}

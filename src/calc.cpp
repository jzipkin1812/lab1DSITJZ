#include <iostream>
#include <string>
#include <vector>
#include "lib/parse.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;

int main()
{
    /*
    vector<vector<Token>> vectors;

        vector<Token> tokens1 = {
            Token(0, 0, "3"),
            Token(0, 0, "+"),
            Token(0, 0, "4"),
            Token(0, 0, "+"),
            Token(0, 0, "5")
        };
        vectors.push_back(tokens1);

        vector<Token> tokens2 = {
            Token(0, 0, "3"),
            Token(0, 0, "+"),
            Token(0, 0, "4"),
            Token(0, 0, "+"),
            Token(0, 0, "5"),
            Token(0, 0, "+"),
            Token(0, 0, "6")
        };
        vectors.push_back(tokens2);

    vector<Token> tokens3 = {
        Token(0, 0, "("),
        Token(0, 0, "3"),
        Token(0, 0, "+"),
        Token(0, 0, "6"),
        Token(0, 0, ")"),
        Token(0, 0, "*"),
        Token(0, 0, "("),
        Token(0, 0, "2"),
        Token(0, 0, "-"),
        Token(0, 0, "4"),
        Token(0, 0, ")"),
        Token(0, 0, "+"),
        Token(0, 0, "7")};
    vectors.push_back(tokens3);
    */
    Lexer myLexer = Lexer();
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print();
    // vector<double> results = myParser.evaluate();
    // for (double result : results)
    //     cout << result << endl;

    return 0;
}
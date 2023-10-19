#include <iostream>
#include <string>
#include <vector>
#include "lib/parser_infix.h"
#include "lib/token.h"

using namespace std;

int main()
{
    vector<vector<Token>> vectors;
/**
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
*/
    vector<Token> tokens3 = {
        Token(0, 0, "3"),
        Token(0, 0, "*"),
        Token(0, 0, "4"),
        Token(0, 0, "+"),
        Token(0, 0, "5")
    };
    vectors.push_back(tokens3);

    ParserInfix myParser = ParserInfix(vectors);
    //vector<double> results = myParser.evaluate();
    //for (double result : results)
        //cout << result << endl;

    return 0;
}

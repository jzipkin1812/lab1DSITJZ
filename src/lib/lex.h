#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <string>
#include <vector>
#include "token.h"
using namespace std;
class Lexer
{
public:
    Lexer(bool pushEnds = false); // does all the complicated building slash lexing
    void print();
    vector<vector<Token>> getTokens() { return (tokens); };
    void parseString(string expression, int lineNumber);
    void turnOnPushEnds() {pushEnds = true;};

private:
    vector<vector<Token>> tokens;
    bool pushEnds;
};

#endif
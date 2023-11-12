#ifndef LEX_H
#define LEX_H

#include <iostream>
#include <string>
#include <vector>
#include "token.h"
#include "typedValue.h"
using namespace std;
class Lexer
{
public:
    Lexer(bool addEnd = false, bool exitImmediately = false, string fileName = ""); // does all the complicated building slash lexing
    void print();
    vector<vector<Token>> getTokens() { return (tokens); };
    void parseString(string expression, int lineNumber);
    void turnOnPushEnds() {pushEnds = true;};

private:
    vector<vector<Token>> tokens;
    bool pushEnds;
    bool exitOnError;
};

#endif
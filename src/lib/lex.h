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
        Lexer(string expression); // does all the complicated building slash lexing
        void print();
        vector<Token> getTokens() {return(tokens);};
    private:
        vector<Token> tokens;
        
};

#endif
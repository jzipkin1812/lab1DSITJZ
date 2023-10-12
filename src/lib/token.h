#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
using namespace std;

struct Token
{
    int line;
    int column; // first character = token number
    string text;

    Token(int l, int c, string t)
    {
        line = l;
        column = c;
        text = t;
    }
    
    bool isOperator()
    {
        return(text == "*" || text == "+" || text == "-" || text == "/");
    }
    bool isParenthesis()
    {
        return(text == ")" || text == "(");
    }
    bool isNumber()
    {
        return(!(isOperator() || isParenthesis()));
    }
};

#endif
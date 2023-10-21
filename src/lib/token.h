#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
#include <cctype>
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
        return(text == "*" || text == "+" || text == "-" || text == "/" || text == "=");
    }
    bool isParenthesis()
    {
        return(text == ")" || text == "(");
    }
    bool isEnd()
    {
        return(text == "END");
    }
    bool isVariable()
    {
        char first = text[0];
        return(!isEnd() && (isalpha(first) || first == '_'));
    }
    bool isNumber()
    {
        return(!(isOperator() || isParenthesis() || isEnd() || isVariable()));
    }
};

#endif
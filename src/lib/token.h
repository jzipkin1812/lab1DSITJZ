#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
#include <cctype>
#include "typedValue.h"
using namespace std;

class Token
{
    public:
        int line;
        int column; // first character = token number
        string text;
        TypeTag type;

        Token(int l, int c, string t)
        {
            line = l;
            column = c;
            text = t;

            if(text == "true" || text == "false") type = BOOLEAN;
            else type = DOUBLE;
        }

        bool isOperator()
        {
            return (text == "*" || text == "+" || text == "-" || text == "/" || text == "=" || text == "%" ||
            text == "<" || text == "!=" || text == ">=" || text == "<=" || text == ">" || text == "|" || text == "&" || text == "^" || text == "==");
        }
        bool takesDoublesOnly()
        {
            return (text == "*" || text == "+" || text == "-" || text == "/" || text == "=" || text == "%" ||
            text == "<" || text == "<=" || text == ">" || text == "<=");
        }
        bool takesBoolsOnly()
        {
            return(text == "|" || text == "&" || text == "^");
        }
        bool isOrderComparison()
        {
            return (text == "<" || text == "<=" || text == ">" || text == "<=");
        }
        bool isBrackets()
        {
            return (text == "]" || text == "[");
        }
        bool isParenthesis()
        {
            return (text == ")" || text == "(");
        }
        bool isEnd()
        {
            return (text == "END");
        }
        bool isVariable()
        {
            char first = text[0];
            return (!(isEnd() || isStatement() || isNull() || isBoolean()) && (isalpha(first) || first == '_'));
        }
        bool isNumber()
        {
            return (!(isBrackets() || isSemicolon() || isNull() || isComma() || isBoolean() || isOperator() || isBrace() || isParenthesis() || isEnd() || isVariable() || isStatement()));
        }
        bool isBoolean()
        {
            return(text == "true" || text == "false");
        }
        bool isOperand()
        {
            return(isBoolean() || isNumber() || isVariable() || isNull());
        }
        bool isStatement()
        {
            return(text == "return" || text == "def" || text == "while" || text == "if" || text == "else" || text == "print");
        }
        bool isBrace()
        {
            return(text == "{" || text == "}");
        }
        bool isComma()
        {
            return(text == ",");
        }
        bool isSemicolon()
        {
            return(text == ";");
        }
        bool isNull()
        {
            return(text == "null");
        }
        
        typedValue getValue()
        {
            typedValue result;
            result.type = type;

            if(type == DOUBLE)
            {
                cout << "Calling stod on: " << text << endl;
                result.data.doubleValue = stod(text);
            }
            else if(type == BOOLEAN)
            {

                result.data.booleanValue = (text == "true");
            }
            return(result);
        }

};

#endif
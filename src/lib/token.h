#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
#include <cctype>
using namespace std;


enum TypeTag 
{
    DOUBLE,
    BOOLEAN
};


struct typedValue
{
    TypeTag type;
    union d
    {
        double doubleValue;
        bool booleanValue;
    };
    d data;

    friend ostream& operator<<(ostream& o, const typedValue& tValue)
    {    
        if(tValue.type == BOOLEAN)
        {
            o << boolalpha << tValue.data.booleanValue;
        }
        else
        {
            o << tValue.data.doubleValue;
        }
        return o;
    };

    inline bool operator==(const typedValue& other) const
    {
        bool result = (other.type == type);
        if(!result) return false;
        if (type == BOOLEAN)
        {
            result = (other.data.booleanValue == data.booleanValue);
        }
        else if (type == DOUBLE)
        {
            result = (other.data.doubleValue == data.doubleValue);
        }
        return(result);
    }
};

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
            text == "<" || text == "!=" || text == "<=" || text == ">" || text == "<=" || text == "|" || text == "&" || text == "^" || text == "==");
        }
        bool isOrderComparison()
        {
            return (text == "<" || text == "<=" || text == ">" || text == "<=");
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
            return (!isEnd() && !isBoolean() && (isalpha(first) || first == '_'));
        }
        bool isNumber()
        {
            return (!(isBoolean() || isOperator() || isParenthesis() || isEnd() || isVariable()));
        }
        bool isBoolean()
        {
            return(text == "true" || text == "false");
        }
        bool isOperand()
        {
            return(isBoolean() || isNumber() || isVariable());
        }
        
        typedValue getValue()
        {
            typedValue result;
            result.type = type;

            if(type == DOUBLE)
            {
                // cout << "Calling stod on: " << text << endl;
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
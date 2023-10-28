#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
#include <cctype>
using namespace std;


enum TypeTag 
{
    DOUBLE, // 0
    BOOLEAN, // 1
    TYPEERROR, // 2
    DIVZEROERROR, // 3
    IDERROR // 4
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
    string unknownIDText;

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

    inline bool operator!=(const typedValue& other) const
    {
        bool result = (other.type != type);
        if(result) return true;
        if (type == BOOLEAN)
        {
            result = (other.data.booleanValue != data.booleanValue);
        }
        else if (type == DOUBLE)
        {
            result = (other.data.doubleValue != data.doubleValue);
        }
        return(result);
    }

    void setType(TypeTag newType)
    {
        if(!isError())
        {
            type = newType;
        }
    }

    bool isError()
    {
        return(type == TYPEERROR || type == DIVZEROERROR || type == IDERROR);
    }

    string outputError()
    {
        string finalOutput = "";
        if(!isError()) return finalOutput;
        else if (type == TYPEERROR) finalOutput = "Runtime error: invalid operand type.\n";
        else if (type == DIVZEROERROR) finalOutput = "Runtime error: division by zero.\n";
        else if (type == IDERROR) finalOutput = "Runtime error: unknown identifier " +  unknownIDText + "\n";
        return(finalOutput);
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
            return (!(isBoolean() || isOperator() || isParenthesis() || isEnd() || isVariable() || isStatement()));
        }
        bool isBoolean()
        {
            return(text == "true" || text == "false");
        }
        bool isOperand()
        {
            return(isBoolean() || isNumber() || isVariable());
        }
        bool isStatement()
        {
            return(text == "while" || text == "if" || text == "else" || text == "print");
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
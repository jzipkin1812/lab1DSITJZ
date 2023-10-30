#ifndef TYPEDVALUE_H
#define TYPEDVALUE_H

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

    string outputError(bool exitImmediately = false)
    {
        string finalOutput = "";
        if(!isError()) return finalOutput;
        else if (type == TYPEERROR) finalOutput = "Runtime error: invalid operand type.\n";
        else if (type == DIVZEROERROR) finalOutput = "Runtime error: division by zero.\n";
        else if (type == IDERROR) finalOutput = "Runtime error: unknown identifier " +  unknownIDText + "\n";
        
        if(exitImmediately)
        {
            cout << finalOutput;
            exit(3);
        }

        return(finalOutput);
    }
};

#endif
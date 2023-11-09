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
    TYPEERROR, // Operator took the wrong type. E.g. true + false or 3 > true
    DIVZEROERROR, // Divide by zero. 3 / 0
    IDERROR, // Unknown identifier. e.g. b + 3 but b was never declared.
    ASSIGNEEERROR, // Invalid assignee, e.g. 3 = 4 or (1 * 2) = a
    NOCONDITIONERROR, // A condition for a statement wasn't a boolean e.g. while(3 + 3) {
    NONE,
    FUNCTION
};

struct typedValue
{
    TypeTag type;
    union d
    {
        double doubleValue;
        bool booleanValue;
        void * functionValue;
    };

    d data;
    string unknownIDText;

    friend ostream& operator<<(ostream& o, const typedValue& tValue)
    {    
        if(tValue.type == BOOLEAN)
        {
            o << boolalpha << tValue.data.booleanValue;
        }
        else if(tValue.type == DOUBLE)
        {
            o << tValue.data.doubleValue;
        }
        else if(tValue.type == NONE)
        {
            o << "null";
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
        return(type == TYPEERROR || type == DIVZEROERROR || type == IDERROR || type == ASSIGNEEERROR || type == NOCONDITIONERROR);
    }

    string outputError(bool exitImmediately = false)
    {
        string finalOutput = "";
        if(!isError()) return finalOutput;
        else if (type == TYPEERROR) finalOutput = "Runtime error: invalid operand type.\n";
        else if (type == DIVZEROERROR) finalOutput = "Runtime error: division by zero.\n";
        else if (type == IDERROR) finalOutput = "Runtime error: unknown identifier " +  unknownIDText + "\n";
        else if (type == ASSIGNEEERROR) finalOutput = "Runtime error: invalid assignee.\n";
        else if (type == NOCONDITIONERROR) finalOutput = "Runtime error: condition is not a bool.\n";
        
        if(exitImmediately)
        {
            cout << finalOutput;
            exit(3);
        }

        return(finalOutput);
    }

};

#endif
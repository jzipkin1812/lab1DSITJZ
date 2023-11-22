#ifndef TYPEDVALUE_H
#define TYPEDVALUE_H

#include <string>
#include <iostream>
#include <cctype>
#include <memory>
#include <vector>
#include <iomanip>
using namespace std;

enum TypeTag
{
    DOUBLE,           // 0
    BOOLEAN,          // 1
    TYPEERROR,        // Operator took the wrong type. E.g. true + false or 3 > true
    DIVZEROERROR,     // Divide by zero. 3 / 0
    IDERROR,          // Unknown identifier. e.g. b + 3 but b was never declared.
    ASSIGNEEERROR,    // Invalid assignee, e.g. 3 = 4 or (1 * 2) = a
    NOCONDITIONERROR, // A condition for a statement wasn't a boolean e.g. while(3 + 3) {
    NONE,
    FUNCTION,
    NOTFUNCTIONERROR, // Not a function e.g. 7(6, 2, 3) or true(9, 10)
    ARGCERROR,        // Incorrect # of arguments passed to function
    BADRETURNERROR,   // Return at top-level, not inside function
    INDEXNOTNUMBERERROR, // [1, 2][true]
    INDEXNOTINTEGERERROR, // [1, 2][0.2]
    SYNTAXERROR,
    NOTARRAYERROR,
    OUTOFBOUNDS,
    INCORRECTARGUMENTCOUNT,
    UNDERFLOW,
    ARRAY             // [1, 2]
};

static string unknownIDText;

struct typedValue
{
    TypeTag type;
    union d
    {
        double doubleValue;
        bool booleanValue;
        vector<typedValue> *arrayValue;
        void *functionValue;
    };

    d data;

    // ~typedValue()
    // {
    //     if (data.arrayValue != nullptr)
    //     {
    //         //delete data.arrayValue;
    //         //data.arrayValue = nullptr;  // Set to nullptr after deletion
    //         cout << "deleted" << endl;
    //     }
    // }

    string toString()
    {
        string result = "";
        if (type == BOOLEAN)
        {
            if (data.booleanValue == true) result += "true";
            else result += "false";
        }
        else if (type == DOUBLE)
        {
            result += data.doubleValue;
        }
        else if (type == ARRAY)
        {
            result += "[";
            for (unsigned int i = 0 ; i < data.arrayValue->size() ; i++)
            {
                result += data.arrayValue->at(i).toString();
                if (i != data.arrayValue->size() - 1)
                    result += ", ";
            }
            result += "]";
        }
        else if (type == NONE)
        {
            result += "null";
        }
        return(result);
    }

    friend ostream &operator<<(ostream &o, const typedValue &tValue)
    {
        if (tValue.type == BOOLEAN)
        {
            o << boolalpha << tValue.data.booleanValue;
        }
        else if (tValue.type == DOUBLE)
        {
            // if (tValue.data.doubleValue < 1e-6)
            //     o << scientific << setprecision(2);
            o << tValue.data.doubleValue;
        }
        else if (tValue.type == ARRAY)
        {
            o << "[";
            // vector<typedValue> * arrPtr = tValue.data.arrayValue;
            for (unsigned int i = 0; i < tValue.data.arrayValue->size(); i++)
            {
                //if (!(tValue.data.arrayValue->at(i).type == NONE))
                o << tValue.data.arrayValue->at(i);
                if (i != tValue.data.arrayValue->size() - 1)
                    o << ", ";
            }
            o << "]";
        }
        else if (tValue.type == NONE)
        {
            o << "null";
        }
        return o;
    };

    inline bool operator==(const typedValue &other) const
    {
        bool result = (other.type == type);
        if (!result)
            return false;
        if (type == BOOLEAN)
        {
            result = (other.data.booleanValue == data.booleanValue);
        }
        else if (type == DOUBLE)
        {
            result = (other.data.doubleValue == data.doubleValue);
        }
        else if (type == ARRAY)
        {
            result = true;
            if (other.data.arrayValue->size() != data.arrayValue->size()) return false;
            for (unsigned int i = 0 ; i < other.data.arrayValue->size() ; i++)
            {
                if (other.data.arrayValue->at(i) != data.arrayValue->at(i)) result = false;
            }
        }
        return (result);
    }

    inline bool operator!=(const typedValue &other) const
    {
        bool result = (other.type != type);
        if (result)
            return true;
        if (type == BOOLEAN)
        {
            result = (other.data.booleanValue != data.booleanValue);
        }
        else if (type == DOUBLE)
        {
            result = (other.data.doubleValue != data.doubleValue);
        }
        return (result);
    }

    void setType(TypeTag newType)
    {
        if (!isError())
        {
            type = newType;
        }
    }

    bool isError()
    {
        return (type == ARGCERROR || type == BADRETURNERROR || type == TYPEERROR || type == NOTFUNCTIONERROR || type == DIVZEROERROR || type == IDERROR || type == ASSIGNEEERROR || type == NOCONDITIONERROR || type == INDEXNOTNUMBERERROR || type == INDEXNOTINTEGERERROR || type == NOTARRAYERROR || type == OUTOFBOUNDS || type == INCORRECTARGUMENTCOUNT || type == UNDERFLOW);
    }

    string outputError(bool exitImmediately = false)
    {
        string finalOutput = "";
        if (!isError())
            return finalOutput;
        else if (type == SYNTAXERROR)
            finalOutput = "Syntax error on line .\n";
        else if (type == UNDERFLOW)
            finalOutput = "Runtime error: underflow.\n";
        else if (type == INCORRECTARGUMENTCOUNT)
            finalOutput = "Runtime error: incorrect argument count.\n";
        else if (type == INDEXNOTINTEGERERROR)
            finalOutput += "Runtime error: index is not an integer.\n";
        else if (type == INDEXNOTNUMBERERROR)
            finalOutput = "Runtime error: index is not a number.\n";
        else if (type == OUTOFBOUNDS)
            finalOutput = "Runtime error: index out of bounds.\n";
        else if (type == TYPEERROR)
            finalOutput = "Runtime error: invalid operand type.\n";
        else if (type == NOTARRAYERROR)
            finalOutput += "Runtime error: not an array.\n";
        else if (type == DIVZEROERROR)
            finalOutput = "Runtime error: division by zero.\n";
        else if (type == IDERROR)
            finalOutput = "Runtime error: unknown identifier " + unknownIDText + "\n";
        else if (type == ASSIGNEEERROR)
            finalOutput = "Runtime error: invalid assignee.\n";
        else if (type == NOCONDITIONERROR)
            finalOutput = "Runtime error: condition is not a bool.\n";
        else if (type == NOTFUNCTIONERROR)
            finalOutput = "Runtime error: not a function.\n";
        else if (type == ARGCERROR)
            finalOutput = "Runtime error: incorrect argument count.\n";
        else if (type == BADRETURNERROR)
            finalOutput = "Runtime error: unexpected return.\n";

        if (exitImmediately)
        {
            cout << finalOutput;
            exit(3);
        }

        return (finalOutput);
    }
};

#endif
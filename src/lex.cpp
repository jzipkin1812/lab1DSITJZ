#include <iostream>
#include <vector>
#include <ctype.h>
#include "lib/lex.h"
#include "lib/parser.h"
#include "lib/token.h"
using namespace std;

void Lexer::print()
{
    for (Token t : tokens)
    {
        cout << t.column << " " << t.line << " " << t.text << endl;
    }
}

Lexer::Lexer(string expression)
{
    int lineNumber = 1;
    int colNumber = 0;
    string currentString = "";
    for (int i = 0; i < (int)expression.length(); i++)
    {
        try
        {
            colNumber++;
            char currentChar = expression[i];
            // cout << "CURRENT CHAR: " << currentChar << endl;
            switch (currentChar)
            {
            case '\n':
                lineNumber++;
                colNumber = 0;
                continue;
            case ' ':
                if (currentString.length() > 0 && currentString[currentString.length() - 1] == '.')
                {
                    throw(colNumber);
                }
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                continue;
            case ')':
            case '(':
                // cout << "CURRENT STRING: " << currentString << endl;
                if (currentString.length() > 0 && currentString[currentString.length() - 1] == '.')
                {
                    throw(colNumber);
                }
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                [[fallthrough]];
            case '+':
            case '-':
            case '*':
            case '/':
                currentString = currentChar;
                // cout << "CURRENT STRING: " << currentString << endl;
                tokens.push_back(Token(lineNumber, colNumber, currentString));
                currentString = "";
                continue;
            default:
                if (currentChar == '.')
                {
                    if (currentString == "")
                    {
                        throw(colNumber);
                    }
                }
                // cout << " IS DIGIT: " << isdigit(currentChar) << endl;
                if (isdigit(currentChar) || currentChar == '.')
                {
                    currentString += currentChar;
                    continue;
                }
                else
                {
                    throw(colNumber);
                }
            }
        }
        catch (int columnNum)
        {
            cout << "Syntax error on line " << lineNumber << " column " << columnNum << "." << endl;
            exit(1);
        }
    }
    tokens.push_back(Token(lineNumber, 1, "END"));
}

int main()
{
    string sExpression = "";
    string sPart;

    getline(cin, sPart);
    while (sPart != "")
    {
        sExpression += (sPart + "\n");
        getline(cin, sPart);
    }
    cout << "SEXPRESSIOM: " << sExpression << endl;
    Lexer myLexer = Lexer(sExpression);
    myLexer.print();
    // Parser myParser = Parser(myLexer.getTokens());
    // myParser.print();
    // cout << "Expression evaluates to: " << myParser.evaluate();

    return (0);
}

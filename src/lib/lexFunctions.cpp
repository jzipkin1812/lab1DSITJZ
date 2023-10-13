#include <iostream>
#include <vector>
#include <iomanip>
#include <ctype.h>
#include "lex.h"
#include "parser.h"
#include "token.h"
using namespace std;

void Lexer::print()
{
    for (Token t : tokens)
    {
        cout << setw(4) << right << t.line << setw(5) << right << t.column << "  " << left << t.text << endl;
    }
}

Lexer::Lexer()
{
    string expression = "";
    getline(cin, expression);
    int lineNumber = 0;
    // int colNumber = 0;
    string currentString = "";
    while (!cin.eof() && expression != "")
    {
        // cout << expression << endl;
        lineNumber++;
        for (int i = 0; i < (int)expression.length(); i++)
        {
            try
            {
                // colNumber++;
                char currentChar = expression[i];
                // cout << "CURRENT CHAR: " << currentChar << endl;
                switch (currentChar)
                {
                /*case '\n':
                    if (currentString != "")
                    {
                        tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                        currentString = "";
                    }
                    lineNumber++;
                    colNumber = 0;
                    continue;*/
                case ')':
                case '(':
                case '+':
                case '-':
                case '*':
                case '/':
                    if (currentString != "")
                    {
                        tokens.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                        currentString = "";
                    }
                    currentString = currentChar;
                    tokens.push_back(Token(lineNumber, i + 1, currentString));
                    currentString = "";
                    continue;
                default:
                    if (isspace(currentChar))
                    {
                        if (currentString != "")
                        {
                            tokens.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                            currentString = "";
                        }
                        continue;
                    }
                    if (currentChar == '.')
                    {
                        // cout << colNumber << endl;
                        // cout << currentString << endl;
                        if ((int)expression.length() == i + 1 || !isdigit(expression[i + 1]))
                        {
                            // colNumber++;
                            throw(i + 2);
                        }
                        if (currentString == "" || (int)currentString.find('.') != -1)
                        {
                            throw(i + 1);
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
                        throw(i + 1);
                    }
                }
            }
            catch (int columnNum)
            {
                cout << "Syntax error on line " << lineNumber << " column " << columnNum << "." << endl;
                exit(1);
            }
        }
        if (currentString != "")
        {
            tokens.push_back(Token(lineNumber, expression.length() + 1 - currentString.length(), currentString));
            currentString = "";
        }
        getline(cin, expression);
    }
    lineNumber++;
    for (int i = 0; i < (int)expression.length(); i++)
    {
        try
        {
            // colNumber++;
            char currentChar = expression[i];
            // cout << "CURRENT CHAR: " << currentChar << endl;
            switch (currentChar)
            {
            /*case '\n':
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                lineNumber++;
                colNumber = 0;
                continue;*/
            case ')':
            case '(':
            case '+':
            case '-':
            case '*':
            case '/':
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                    currentString = "";
                }
                currentString = currentChar;
                tokens.push_back(Token(lineNumber, i + 1, currentString));
                currentString = "";
                continue;
            default:
                if (isspace(currentChar))
                {
                    if (currentString != "")
                    {
                        tokens.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                        currentString = "";
                    }
                    continue;
                }
                if (currentChar == '.')
                {
                    // cout << colNumber << endl;
                    // cout << currentString << endl;
                    if ((int)expression.length() == i + 1 || !isdigit(expression[i + 1]))
                    {
                        // colNumber++;
                        throw(i + 2);
                    }
                    if (currentString == "" || (int)currentString.find('.') != -1)
                    {
                        throw(i + 1);
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
                    throw(i + 1);
                }
            }
        }
        catch (int columnNum)
        {
            cout << "Syntax error on line " << lineNumber << " column " << columnNum << "." << endl;
            exit(1);
        }
    }
    if (currentString != "")
    {
        tokens.push_back(Token(lineNumber, expression.length() + 1 - currentString.length(), currentString));
        currentString = "";
    }
    tokens.push_back(Token(lineNumber, expression.length() + 1, "END"));
}

// int main()
// {
//     // cout << sExpression << endl;
//     Lexer myLexer = Lexer();
//     myLexer.print();
//     // Parser myParser = Parser(myLexer.getTokens());
//     // myParser.print();
//     // cout << "Expression evaluates to: " << myParser.evaluate();

//     return (0);
// }
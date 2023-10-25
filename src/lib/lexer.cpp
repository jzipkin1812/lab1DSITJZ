#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "token.h"
#include "lex.h"

void Lexer::print()
{
    for (vector<Token> expression : tokens)
    {
        for (Token t : expression)
        {
            cout << setw(4) << right << t.line << setw(5) << right << t.column << "  " << left << t.text << endl;
        }
    }
}

Lexer::Lexer(bool addEnd) // time complexity O(n^2), (number of lines) X (number of characters in each line)
{
    pushEnds = addEnd;
    string expression = ""; // expression is set equal to each new line read by cin
    getline(cin, expression);
    int lineNumber = 0;
    while (!cin.eof() && expression != "")
    {
        lineNumber++;
        parseString(expression, lineNumber); // parseString() breaks each line into tokens and pushes them to the tokens vector
        getline(cin, expression);
    }
    lineNumber++;
    parseString(expression, lineNumber); // parseString runs one more time after cin.eof() in the case of an eof being located on the same line as an expression
    tokens.back().push_back(Token(lineNumber, expression.length() + 1, "END")); //tokens: vector of vectors, each vector contains a new expression
}

void Lexer::parseString(string expression, int lineNumber) // time complexity O(n), n=characters in expression
{
    string currentString = ""; // currentString gets set to each token and pushed to tokens vector
    vector<Token> currentExpression;
    for (int i = 0; i < (int)expression.length(); i++) // (int) cast necessary when comparing int and unsigned int
    {
        try
        {
            char currentChar = expression[i];
            switch (currentChar)
            {
            case ')': // intentional fallthrough
            case '(':
            case '+':
            case '-':
            case '*':
            case '/':
            case '=':
            case '=':
                if (currentString != "")
                {
                    currentExpression.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                    currentString = "";
                }
                currentString = currentChar;
                currentExpression.push_back(Token(lineNumber, i + 1, currentString));
                currentString = "";
                continue;
            default: // case of spaces, digits, and '.'
                if (isspace(currentChar))
                {
                    if (currentString != "")
                    {
                        currentExpression.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                        currentString = "";
                    }
                    continue;
                }
                if (currentChar == '.')
                {
                    if ((int)expression.length() == i + 1 || !isdigit(expression[i + 1]))
                    {
                        throw(i + 2);
                    }
                    if (currentString == "" || (int)currentString.find('.') != -1)
                    {
                        throw(i + 1);
                    }
                }
                if (isdigit(currentChar))
                {

                    if ((int)currentString.length() > 0 && !(isdigit(currentString[0]) || isalpha(currentString[0]) || currentString[0] == '_'))
                    {
                        throw(i + 1);
                    }
                    currentString += currentChar;
                    continue;
                }
                if (currentChar == '.')
                {
                    if ((int)currentString.length() > 0 && !isdigit(currentString[0]))
                    {
                        throw(i + 1);
                    }
                    currentString += currentChar;
                    continue;
                }
                else if (isalpha(currentChar) || currentChar == '_')
                {
                    if ((int)currentString.length() > 0 && isdigit(currentString[0]))
                    {
                        throw(i + 1);
                    }
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
            
            if(pushEnds)
            {
                cout << "Syntax error on line " << 1 << " column " << columnNum << "." << endl;
                return;
            }
            else
            {
                cout << "Syntax error on line " << lineNumber << " column " << columnNum << "." << endl;
                exit(1);
            }
        }
    }
    if (currentString != "") // case for when a number is located directly before '\n'
    {
        currentExpression.push_back(Token(lineNumber, expression.length() + 1 - currentString.length(), currentString));
        currentString = "";
    }
    // Take the entire expression and push it to the 2D vector
    // For the new parser, we want ENDs everywhere
    if(pushEnds)
    {
        currentExpression.push_back(Token(lineNumber, expression.length() + 1, "END"));
    } 
    tokens.push_back(currentExpression);
}
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
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

Lexer::Lexer(bool addEnd, bool exitImmediately, string fileName, bool checkSyntaxErrors) // time complexity O(n^2), (number of lines) X (number of characters in each line)
{
    checkErrors = checkSyntaxErrors;
    // Handle text files
    ifstream fileStream;
    if(fileName != "") fileStream = ifstream(fileName);
    
    pushEnds = addEnd;
    exitOnError = exitImmediately;
    string expression = ""; // expression is set equal to each new line read by cin
    if(fileName == "") getline(cin, expression);
    else getline(fileStream, expression);
    
    int lineNumber = 0;
    int count = 0; // when count reaches 2 (two consecutive empty lines), the program should stop asking for input
    bool endOfFile = cin.eof();
    if(fileName != "") endOfFile = fileStream.eof();
    while (!endOfFile && count < 2)
    {
        lineNumber++;
        if (expression == "") count++;
        else count = 0;
        parseString(expression, lineNumber); // parseString() breaks each line into tokens and pushes them to the tokens vector
        if(fileName == "")
        {
            getline(cin, expression);
            endOfFile = cin.eof();
        }
        else
        {
            getline(fileStream, expression);
            endOfFile = fileStream.eof();
        }
    }
    lineNumber++;
    parseString(expression, lineNumber);                                        // parseString runs one more time after cin.eof() in the case of an eof being located on the same line as an expression
    if (expression != "")
    {
        parseString("", lineNumber + 1);
    }
    tokens.back().push_back(Token(lineNumber, expression.length() + 1, "END")); // tokens: vector of vectors, each vector contains a new expression
    fileStream.close();
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
            case ';':
            case ',':
            case '*':
            case '/':
            case '^':
            case '}':
            case '{':
            case '|':
            case '&':
            case '[':
            case ']':
            case '%':
                if (currentString != "")
                {
                    currentExpression.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                    currentString = "";
                }
                currentString = currentChar;
                currentExpression.push_back(Token(lineNumber, i + 1, currentString));
                currentString = "";
                continue;
            case '=':
            case '<':
            case '>':
                if (currentString != "")
                {
                    currentExpression.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                    currentString = "";
                }
                currentString = expression[i + 1] == '=' ? string(1, currentChar) + "=" : string(1, currentChar);
                if (currentString.length() == 2)
                    i++;
                currentExpression.push_back(Token(lineNumber, i + 2 - currentString.length(), currentString));
                currentString = "";
                continue;
            case '!':
                if (currentString != "")
                {
                    currentExpression.push_back(Token(lineNumber, i + 1 - currentString.length(), currentString));
                    currentString = "";
                }
                if (expression[i + 1] == '=')
                    currentString = "!=";
                else
                {
                    throw(i + 1);
                }
                if (currentString.length() == 2)
                    i++;
                currentExpression.push_back(Token(lineNumber, i + 2 - currentString.length(), currentString));
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
            if (checkErrors)
            {
                if (pushEnds)
                {
                    cout << "Syntax error on line " << 1 << " column " << columnNum << "." << endl;
                }
                else
                {
                    cout << "Syntax error on line " << lineNumber << " column " << columnNum << "." << endl;
                }
                if(exitOnError) exit(1);
                else return;
            }
            else // only used for calc
            {
                if (pushEnds)
                {
                    currentExpression.push_back(Token(1, columnNum, ""));
                }
                else
                {
                    currentExpression.push_back(Token(lineNumber, columnNum, ""));
                }
                if (exitOnError)
                    exit(1);
                else
                {
                    if (pushEnds)
                    {
                        currentExpression.push_back(Token(lineNumber, expression.length() + 1, "END"));
                    }
                    tokens.push_back(currentExpression);
                }
                return;
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
    if (pushEnds)
    {
        currentExpression.push_back(Token(lineNumber, expression.length() + 1, "END"));
    }
    tokens.push_back(currentExpression);
}


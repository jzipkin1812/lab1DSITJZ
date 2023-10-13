#include <iostream>
#include <vector>
#include <iomanip>
#include <ctype.h>
#include "lib/lex.h"
#include "lib/parser.h"
#include "lib/token.h"
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
    while (!cin.eof() && expression != "")
    {
        lineNumber++;
        parseString(expression, lineNumber);
        getline(cin, expression);
    }
    lineNumber++;
    parseString(expression, lineNumber);
    tokens.push_back(Token(lineNumber, expression.length() + 1, "END"));
}

void Lexer::parseString(string expression, int lineNumber)
{
    string currentString = "";
    for (int i = 0; i < (int)expression.length(); i++)
    {
        try
        {
            char currentChar = expression[i];
            switch (currentChar)
            {
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
                    if ((int)expression.length() == i + 1 || !isdigit(expression[i + 1]))
                    {
                        throw(i + 2);
                    }
                    if (currentString == "" || (int)currentString.find('.') != -1)
                    {
                        throw(i + 1);
                    }
                }
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
}

int main()
{
    Lexer myLexer = Lexer();
    myLexer.print();
    // Parser myParser = Parser(myLexer.getTokens());
    // myParser.print();
    // cout << "Expression evaluates to: " << myParser.evaluate();

    return (0);
}

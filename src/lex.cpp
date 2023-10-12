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
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                lineNumber++;
                colNumber = 0;
                continue;
            case ' ':
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                continue;
            case ')':
            case '(':
            case '+':
            case '-':
            case '*':
            case '/':
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                currentString = currentChar;
                tokens.push_back(Token(lineNumber, colNumber, currentString));
                currentString = "";
                continue;
            default:
                if (currentChar == '.')
                {
                    // cout << colNumber << endl;
                    if ((int)expression.length() == i + 1 || !isdigit(expression[i + 1]))
                    {
                        colNumber--;
                        throw(colNumber);
                    }
                    if (currentString == "" || (int)currentString.find('.') != -1)
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
    while (sPart != "" && sPart != " " && sPart != " \n")
    {
        sExpression += (sPart + "\n");
        getline(cin, sPart);
    }
    // cout << sExpression << endl;
    Lexer myLexer = Lexer(sExpression);
    myLexer.print();
    // Parser myParser = Parser(myLexer.getTokens());
    // myParser.print();
    // cout << "Expression evaluates to: " << myParser.evaluate();

    return (0);
}

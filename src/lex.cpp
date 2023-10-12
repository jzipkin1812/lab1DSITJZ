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
        cout << t.line << " " << t.column << " " << t.text << endl;
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
            cout << "CURRENT CHAR: " << currentChar << endl;
            switch (currentChar)
            {
            case 'n':
                if (i > 0 && expression[i - 1] == '\\')
                {
                    lineNumber++;
                    colNumber = 0;
                    continue;
                }
                else
                {
                    throw(i);
                }
            case '\\':
                if (i + 1 < (int)expression.length() && expression[i + 1] == 'n')
                {
                    continue;
                }
                else
                {
                    throw(i);
                }
            case ' ':
                if (currentString.length() > 0 && currentString[currentString.length() - 1] == '.')
                {
                    throw(i);
                }
                if (currentString != "")
                {
                    tokens.push_back(Token(lineNumber, colNumber - currentString.length(), currentString));
                    currentString = "";
                }
                continue;
            case ')':
            case '(':
                if (currentString.length() > 0 && currentString[currentString.length() - 1] == '.')
                {
                    throw(i);
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
                tokens.push_back(Token(lineNumber, colNumber, currentString));
                currentString = "";
                continue;
            default:
                if (currentChar == '.')
                {
                    if (currentString == "")
                    {
                        throw(i);
                    }
                }
                cout << " IS DIGIT: " << isdigit(currentChar) << endl;
                if (isdigit(currentChar) || currentChar == '.')
                {
                    currentString += currentChar;
                    continue;
                }
                else
                {
                    throw(i);
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
    string sExpression;

    // cout << "Input the S-expression: ";
    // getline(cin, sExpression);

    // Lexer myLexer = Lexer(sExpression);
    // myLexer.print();
    // vector<Token> sTokens = {
    //     Token(1, 0, "("),
    //     Token(2, 0, "*"),
    //     Token(3, 0, "("),
    //     Token(4, 0, "+"),
    //     Token(5, 0, "1"),
    //     Token(6, 0, "2"),
    //     Token(7, 0, ")"),
    //     Token(8, 0, "3"),
    //     Token(9, 0, "("),
    //     Token(10, 0, "/"),
    //     Token(11, 0, "4"),
    //     Token(12, 0, "5"),
    //     Token(13, 0, "("),
    //     Token(14, 0, "-"),
    //     Token(15, 0, "6"),
    //     Token(16, 0, "7"),
    //     Token(17, 0, ")"),
    //     Token(18, 0, ")"),
    //     Token(19, 0, ")"),
    //     Token(20, 0, "END")
    // };
    // Parser myParser = Parser(sTokens);
    // myParser.print();
    // cout << "Expression evaluates to: " << myParser.evaluate();

    return (0);
}


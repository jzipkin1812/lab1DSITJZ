#include <iostream>
#include <string>
#include "lib/parser.h"
#include "lib/token.h"
#include "lib/lex.h"
#include "lex.cpp"
using namespace std;

Parser::Parser(vector<Token> inTokens)
{
    tokens = inTokens;
    // cout << "constructor reached" << endl;
    // Handle the case where the entire expression is just one number with nothing
    if(inTokens.size() == 2 && inTokens[0].isNumber())
    {
        root = new Node{Parser::Node{tokens[0], vector<Node*>(), nullptr}};
        return;
    }
    //Handle no expression
    if(inTokens.size() < 2)
    {
        parseError(tokens[0]);
    }
    
    int openParentheses = 1;
    // Handle error where first token is left parenthesis and second token is number or first token is not left parenthesis
    if(tokens[0].text != "(")
    {
        parseError(tokens[0]);
    }
    else if(!tokens[1].isOperator())
    {
        parseError(tokens[1]);
    }
    root = new Node{Parser::Node{tokens[1], vector<Node*>(), nullptr}};
    Node * currentPtr = root;
    
    for(unsigned int tIndex = 2; tIndex < tokens.size(); tIndex++)
    {
        Token currentToken = tokens[tIndex];
        if(currentToken.text == "(")
        {
            tIndex++; // The current token should now be an operator
            currentToken = tokens[tIndex];
            // If the current token is not an operator, then we have encountered an unexpected token
            // This is a parse error.
            if (!(currentToken.isOperator()))
            {
                parseError(currentToken);
            }
            // Create a new subtree
            Node * operatorNode = new Node{Parser::Node{tokens[tIndex], vector<Node*>(), currentPtr}};
            currentPtr->branches.push_back(operatorNode);
            currentPtr = operatorNode;
            openParentheses++;
        }
        
        else if(currentToken.text == ")")
        {
            // Handle unbalanced parentheses
            // Also handle operators that have been placed right in front of a closed parentheses
            if(openParentheses < 1 || tokens[tIndex - 1].isOperator())
            {
                parseError(currentToken);
            }
            else
            {
                openParentheses--;
            }
            // Go up a level
            currentPtr = currentPtr->parent;
            
        }
        // This error covers the case where we find an operator on its own. The only way to reach this if statement
        // is if the operator found did not come after an open parenthesis. All S-expressions should have operators
        // after open parentheses, so this is a parse error. Operators are normally handled above just after an open
        // parenthesis is reached.
        else if(currentToken.isOperator())
        {
            parseError(currentToken);
        }
        else if(currentToken.text == "END")
        {
            // Check all parentheses are closed
            if(openParentheses != 0)
            {
                parseError(currentToken);
            }
        }
        // Otherwise its a number
        else
        {
            currentPtr->branches.push_back(new Node{Parser::Node{currentToken, vector<Node*>(), currentPtr}});
        }

    }

}

void Parser::print() // Infix
{
    if(!root)
    {
        return;
    }
    string finalOutput = printHelper(root, true);
    cout << finalOutput << endl << evaluate() << endl;
}

string Parser::printHelper(Parser::Node * top, bool lastChild)
{
    string finalText = "";
    bool last;
    if(top->info.isOperator())
    {
        finalText += "(";
        for(unsigned int i = 0; i < top->branches.size(); i++)
        {
            last = (i == top->branches.size() - 1);
            finalText += printHelper(top->branches[i], last);
        }
        finalText += ")";
        if(!lastChild)
        {
            finalText += " " + top->parent->info.text + " ";
        }
    }
    else
    {
        // cout << "print helper reached " << top->info.text << endl;
        string converted = top->info.text;
        converted.erase (converted.find_last_not_of('0') + 1, std::string::npos );
        converted.erase (converted.find_last_not_of('.') + 1, std::string::npos );
        finalText += converted;
        
        if(!lastChild)
        {
            // Space, parent operator, space
            finalText += " " + top->parent->info.text + " ";
        }
    }
    return(finalText);
}

double Parser::evaluate()
{
    return(evaluateHelper(root));
}

double Parser::evaluateHelper(Node * top)
{
    double result = 0;
    string text = top->info.text;
    if(text == "+")
    {
        for(Node * child : top->branches)
        {
            result += evaluateHelper(child);
        }
    }
    else if(text == "-")
    {
        result = evaluateHelper(top->branches[0]);
        for(unsigned int i = 1; i < top->branches.size(); i++)
        {
            result -= evaluateHelper(top->branches[i]);
        }
    }
    else if(text == "*")
    {
        result = 1;
        for(Node * child : top->branches)
        {
            result *= evaluateHelper(child);
        }
    }
    else if(text == "/")
    {
        result = evaluateHelper(top->branches[0]);
        for(unsigned int i = 1; i < top->branches.size(); i++)
        {
            // Divide, but check for division by 0 error
            double divisor = evaluateHelper(top->branches[i]);
            if(divisor == 0)
            {
                cout << "Runtime error: division by zero." << endl;
                exit(3);
            }
            result /= divisor;
        }
    }
    else
    {
        // cout << "evaluate helper reached [" << text << "]" << endl;
        result = stod(text);
    }
    return(result);
}   

void Parser::parseError(Token token)
{
    cout << "Unexpected token at line "<< token.line << " column " << token.column << ": " << token.text << endl;
    exit(2);
}

Parser::~Parser()
{
    clear(root);
}

void Parser::clear(Node * top)
{
    for(Node * child : top->branches)
        {
            clear(child);
        }
    delete top;
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
    for(Token t : myLexer.getTokens())
    {
        cout << t.text << " ";
    }
    cout << endl;
    Parser myParser = Parser(myLexer.getTokens());
    myParser.print();
}
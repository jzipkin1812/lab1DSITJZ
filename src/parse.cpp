#include <iostream>
#include <string>
#include "lib/parser.h"
#include "lib/token.h"
using namespace std;

Parser::Parser(vector<Token> inTokens)
{
    tokens = inTokens;
    int openParentheses = 1;
    root = new Node{Parser::Node{tokens[1], vector<Node*>(), nullptr}};
    Node * currentPtr = root;
    
    for(unsigned int tIndex = 2; tIndex < tokens.size(); tIndex++)
    {
        Token currentToken = tokens[tIndex];
        if(currentToken.text == "(")
        {
            tIndex++; // The current token should now be an operator
            currentToken = tokens[tIndex];
            // If the current token is not an operator, then we have encountered an unexpected token.
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
            if(openParentheses < 1)
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
        finalText += top->info.text;
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
                exit(2);
            }
            result /= divisor;
        }
    }
    else
    {
        result = stod(text);
    }
    return(result);
}   

void Parser::parseError(Token token)
{
    cout << "Unexpected token at line "<< token.line << " column " << token.column << ": " << token.text << endl;
    exit(2);
}

int main()
{
    return(0);
}
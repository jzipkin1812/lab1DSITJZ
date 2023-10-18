#include <iostream>
#include <string>
#include <vector>
#include "token.h"
#include "lex.h"
#include "parser.h"
using namespace std;

Parser::Parser(vector<vector<Token>> inputFromLexer)
{
    // cout << inputFromLexer.size() << endl;
    // for(auto token : inputFromLexer.back())
    // {
    //     cout << token.text << " ";
    // }
    // cout << endl;
    for(auto expression : inputFromLexer)
    {
        roots.push_back(constructAST(expression));
    }
    // Delete any vectors that are nullptr
    while(roots.back() == nullptr)
    {
        roots.pop_back();
    }
    
}

Parser::Node * Parser::constructAST(vector<Token> tokens)
{

    Node * root = nullptr;
    // Handle the case where the entire expression is just an end token
    if(tokens.size() == 1)
    {
        return nullptr;
    }
    // Handle the case where the entire expression is just one number with nothing
    if(tokens.size() == 2 && tokens[0].isNumber())
    {
        root = new Node{Parser::Node{tokens[0], vector<Node*>(), nullptr}};
        return(root);
    }
    //Handle no expression
    if(tokens.size() < 2)
    {
        if (tokens[0].line != 1) 
            tokens[0].line++;
        parseError(tokens[0]);
    }
    
    int openParentheses = 1;
    // Handle error where first token is left parenthesis and second token is number or first token is not left parenthesis
    if(tokens[0].text != "(")
    {
        if(tokens[0].isNumber())
        {
            parseError(tokens[1]);
        }
        else
        {
            parseError(tokens[0]);
        }
        
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
            // If the current pointer is null, then we have reached a Multiple Expressions error.
            if(!currentPtr)
            {
                parseError(currentToken);
            }
            
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
    return(root);
}

void Parser::print() // Infix
{
    for(Node * root : roots)
    {
        string finalOutput = printHelper(root, true);
        cout << finalOutput << endl << evaluate(root) << endl;
    }
    // DEBUG: Printing variable values
    // auto v = variables.begin();
    // while(v != variables.end())
    // {
    //     cout << v->first << " : " << v->second << endl;
    //     v++;
    // }
    
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
        string converted = top->info.text;
        // Formatting removes trailing 0s
        if(converted != "0")
        {
            converted.erase (converted.find_last_not_of('0') + 1, std::string::npos );
            converted.erase (converted.find_last_not_of('.') + 1, std::string::npos );
        }
        
        finalText += converted;
        
        if(!lastChild)
        {
            // Space, parent operator, space
            finalText += " " + top->parent->info.text + " ";
        }
    }
    return(finalText);
}

double Parser::evaluate(Node * top)
{
    double result = 0;
    Token t = top->info;
    string text = top->info.text;
    if(text == "+")
    {
        for(Node * child : top->branches)
        {
            result += evaluate(child);
        }
    }
    else if(text == "-")
    {
        result = evaluate(top->branches[0]);
        for(unsigned int i = 1; i < top->branches.size(); i++)
        {
            result -= evaluate(top->branches[i]);
        }
    }
    else if(text == "*")
    {
        result = 1;
        for(Node * child : top->branches)
        {
            result *= evaluate(child);
        }
    }
    else if(text == "/")
    {
        result = evaluate(top->branches[0]);
        for(unsigned int i = 1; i < top->branches.size(); i++)
        {
            // Divide, but check for division by 0 error
            double divisor = evaluate(top->branches[i]);
            if(divisor == 0)
            {
                cout << "Runtime error: division by zero." << endl;
                exit(3);
            }
            result /= divisor;
        }
    }
    // The assignment operator is right-associative, so it evaluates the last (rightmost) child
    // of the operator in the AST to figure out what to assign these variables to.
    else if(text == "=")
    {
        // Get the rightmost value recursively
        result = evaluate(top->branches[top->branches.size() - 1]);
        // Assign this value to all the variables
        for(unsigned int i = 0; i < top->branches.size() - 1; i++)
        {
            variables[top->branches[i]->info.text] = result;
        }
    }
    else if(t.isNumber())
    {
        result = stod(text);
    }
    else if(t.isVariable())
    {
        // Test for undefined identifier error
        if(variables.find(text) == variables.end())
        {
            cout << "Runtime error: unknown identifier " << text << endl;
            exit(3);
        }
        else
        {
            result = variables[text];
        }
    }
    else
    {
        cout << "Encountered a variable " << text << endl;
        exit(2);
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
    for(Node * root : roots)
    {
        clear(root);
    }
}

void Parser::clear(Node * top)
{
    for(Node * child : top->branches)
    {
        clear(child);
    }
    delete top;
}

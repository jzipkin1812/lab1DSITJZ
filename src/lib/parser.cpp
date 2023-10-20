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
    // tokens = inputFromLexer;
    for(vector<Token> line : inputFromLexer)
    {
        for(Token t : line)
        {
            singleTokens.push_back(t);
        }
    }
    while(singleTokens.size() > 0)
    {
        vector<Token> oneExp = oneExpression();
        // for(Token t: oneExp)
        // {
        //     cout << "[" << t.text << "]";
        // }
        // cout << endl;
        tokens.push_back(oneExp);
    }

    for(auto expression : tokens)
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
    // Handle the case where the entire expression is just a parenthesis
    if(tokens.size() == 1 && (tokens[0].isOperator() || tokens[0].isParenthesis()))
    {
        parseError(tokens[0]);
    }
    // Handle the case where the entire expression is just an end token or nothing
    if(tokens.size() == 0 || (tokens.size() == 1 && tokens[0].isEnd()))
    {
        return nullptr;
    }
    // Handle the case where the entire expression is just one number or just one variable with nothing
    if((tokens.size() == 1 && (tokens[0].isNumber() || tokens[0].isVariable())) || 
    (tokens.size() == 2 && (tokens[0].isNumber() || tokens[0].isVariable()) && tokens[1].isEnd()))
    {
        root = new Node{Parser::Node{tokens[0], vector<Node*>(), nullptr}};
        return(root);
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
            // If the current pointer points to an assignment operator
            // we have an Invalid Assignee error.
            // if(currentPtr->info.text == "=" && !tokens[tIndex + 1].isVariable())
            // {
            //     parseError(currentToken);
            // }
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
            // Handle variables that have been assigned to nothing
            if(tokens[tIndex - 1].isVariable() && tokens[tIndex - 2].text == "=")
            {
                parseError(currentToken);
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
        else if(currentToken.isNumber() || currentToken.isVariable())
        {
            currentPtr->branches.push_back(new Node{Parser::Node{currentToken, vector<Node*>(), currentPtr}});
        }

        // End of expression
        if((currentToken.text == "END" || tIndex == tokens.size() - 1) && openParentheses != 0)
        {
            parseError(currentToken);
        }
    }
    return(root);
}

void Parser::print() // Infix
{
    for(Node * root : roots)
    {
        double finalValue = evaluate(root);
        string finalInfix = printHelper(root, true);
        cout << finalInfix << endl << finalValue << endl;
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
    if(!top) return finalText;
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
        if(converted != "0" && converted.find('.') != string::npos)
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
    if(!top)
    {
        return 0;
    }
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
                // Find the root of the tree and print infix version (the tree should still print in infix form when a runtime error occurs!)
                while(top->parent)
                {
                    top = top->parent;
                }
                cout << printHelper(top, true) << endl;
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
        // for(int i = top->branches.size() - 2; i >= 0; i--)
        // First, check for assignee errors.
        for(unsigned int i = 0; i < top->branches.size() - 1; i++)
        {
            Token assignee = top->branches[i]->info;
            // invalid assignees are not variables.
            if(!assignee.isVariable())
            {
                // Note that OPERATORS are not thrown, the parentheses before them are.
                // if the first child was bad, it is thrown.
                if(i == 0)
                {
                    if(assignee.isOperator())
                    {
                        parseError(findParenthesisBefore(assignee));
                    }
                    else
                    {
                        parseError(assignee);
                    }
                }
                // If any other child was bad, then the last child is thrown.
                else
                {
                    Token lastChild = top->branches[top->branches.size() - 1]->info;
                    if(lastChild.isOperator())
                    {
                        parseError(findParenthesisBefore(lastChild));
                    }
                    else
                    {
                        parseError(lastChild);
                    }
                }
            }
        }
        // If there were no assignee errors, assign all the operands to the value of the rightmost expression.
        result = evaluate(top->branches[top->branches.size() - 1]);
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
    if(!top)
    {
        return;
    }
    for(Node * child : top->branches)
    {
        clear(child);
    }
    delete top;
}

Token Parser::findParenthesisBefore(Token o)
{
    vector<Token> line = tokens[o.line - 1];
    for(unsigned int i = 0; i < line.size(); i++)
    {
        if(line[i].column == o.column)
        {
            return(line[i - 1]);
        }
    }
    // SHOULD NEVER BE REACHED
    return(o);
}

vector<Token> Parser::oneExpression()
{
    int parentheses = 0;
    vector<Token> result;
    while(singleTokens.size() > 0)
    {
        Token t = singleTokens.front();
        if(t.text == "(")
        {
            parentheses++;
        }
        else if(t.text == ")")
        {
            parentheses--;
            // Find the end of the expression via PARENTHESES
            if(parentheses == 0)
            {
                result.push_back(t);
                singleTokens.erase(singleTokens.begin());
                return(result);
            }
        }
        // Find the end of the expression via ONE SINGULAR NUMBER OR IDENTIFIER
        else if((t.isVariable() || t.isNumber()) && parentheses == 0)
        {
            result.push_back(t);
            singleTokens.erase(singleTokens.begin());
            return(result);
        }
        result.push_back(t);
        singleTokens.erase(singleTokens.begin());
    }
    return(result);
}

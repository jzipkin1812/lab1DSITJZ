#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include "token.h"
#include "lex.h"
#include "parse.h"
using namespace std;

Parser::Parser(vector<vector<Token>> inputFromLexer)
{
    for (auto expression : inputFromLexer)
    {
        if (expression.back().text == "END")
        {
            break;
        }
        roots.push_back(constructAST(expression));
    }
    // Delete any vectors that are nullptr
    while (roots.back() == nullptr)
    {
        roots.pop_back();
    }
}

int getPrecedence(string token)
{
    if (token == "=")
        return (1);
    else if (token == "+" || token == "-")
        return (2);
    else if (token == "*" || token == "/")
        return (3);
    else
        return (4);
}

Parser::Node *Parser::constructAST(vector<Token> tokens)
{
    stack<Node *> nodeStack;
    stack<string> stringStack;
    Node *root = nullptr;
    Node *child1 = nullptr;
    Node *child2 = nullptr;
    if (tokens.size() == 0)
    {
        return nullptr;
    }
    for (unsigned int i = 0; i < tokens.size(); i++)
    {
        // cout << tokens[i].text << endl;
        if (tokens[i].text == "(")
        {
            stringStack.push("(");
        }
        else if (tokens[i].isNumber() || tokens[i].isVariable())
        {
            nodeStack.push(new Node{Parser::Node{tokens[i], vector<Node *>(), nullptr}});
            if (i == tokens.size() - 1 && !stringStack.empty())
            {
                while (!stringStack.empty() 
                    && stringStack.top() != "(")
                {
                    string currentString = stringStack.top();
                    root = new Node{
                        Parser::Node{Token{0, 0, currentString}, vector<Node *>(), nullptr}};
                    stringStack.pop();
                    child1 = nodeStack.top();
                    nodeStack.pop();
                    child2 = nodeStack.top();
                    nodeStack.pop();
                    root->branches.push_back(child2);
                    child2->parent = root;
                    root->branches.push_back(child1);
                    child1->parent = root;
                    nodeStack.push(root);
                }
            }
        }
        else if (tokens[i].text != ")")
        {
            while (!stringStack.empty() && 
                    stringStack.top() != "(" 
                    && getPrecedence(stringStack.top()) >= getPrecedence(tokens[i].text))
            {
                string currentString = stringStack.top();
                root = new Node{
                    Parser::Node{Token{0, 0, currentString}, vector<Node *>(), nullptr}};
                stringStack.pop();
                child1 = nodeStack.top();
                nodeStack.pop();
                child2 = nodeStack.top();
                nodeStack.pop();
                root->branches.push_back(child2);
                child2->parent = root;
                root->branches.push_back(child1);
                child1->parent = root;
                nodeStack.push(root);
            }
            stringStack.push(tokens[i].text);
        }
        else if (tokens[i].text == ")")
        {
            while (!stringStack.empty() && stringStack.top() != "(")
            {
                string currentString = stringStack.top();
                root = new Node{
                    Parser::Node{Token{0, 0, currentString}, vector<Node *>(), nullptr}};
                stringStack.pop();
                child1 = nodeStack.top();
                nodeStack.pop();
                child2 = nodeStack.top();
                nodeStack.pop();
                root->branches.push_back(child2);
                child2->parent = root;
                root->branches.push_back(child1);
                child1->parent = root;
                // cout << "PUSHING:" << root->info.text << root->branches[0]->info.text << root->branches[1]->info.text << endl;
                nodeStack.push(root);
            }
            if (!stringStack.empty())
            {
                stringStack.pop();
            }
        }
    }
    Node *finalRoot = nodeStack.top();
    // cout << finalRoot->info.text << endl;
    return finalRoot;
}

void Parser::print() // Infix
{
    for (Node *root : roots)
    {
        double finalValue = evaluate(root);
        // cout << root->info.text << endl;
        string finalInfix = printHelper(root, true);
        cout << finalInfix << endl;
        cout << finalValue << endl;
        // << finalValue << endl;
    }
    // DEBUG: Printing variable values
    // auto v = variables.begin();
    // while(v != variables.end())
    // {
    //     cout << v->first << " : " << v->second << endl;
    //     v++;
    // }
}

string Parser::printHelper(Parser::Node *top, bool lastChild)
{
    string finalText = "";
    if (!top)
        return finalText;
    bool last;
    if (top->info.isOperator())
    {
        finalText += "(";
        for (unsigned int i = 0; i < top->branches.size(); i++)
        {
            last = (i == top->branches.size() - 1);
            finalText += printHelper(top->branches[i], last);
        }
        finalText += ")";
        if (!lastChild)
        {
            finalText += " " + top->parent->info.text + " ";
        }
    }
    else
    {
        string converted = top->info.text;
        // Formatting removes trailing 0s
        if (converted != "0" && converted.find('.') != string::npos)
        {
            converted.erase(converted.find_last_not_of('0') + 1, std::string::npos);
            converted.erase(converted.find_last_not_of('.') + 1, std::string::npos);
        }

        finalText += converted;

        if (!lastChild)
        {
            // Space, parent operator, space
            finalText += " " + top->parent->info.text + " ";
        }
    }
    return (finalText);
}

double Parser::evaluate(Node *top)
{
    if (!top)
    {
        return 0;
    }
    double result = 0;
    Token t = top->info;
    string text = top->info.text;
    if (text == "+")
    {
        for (Node *child : top->branches)
        {
            result += evaluate(child);
        }
    }
    else if (text == "-")
    {
        result = evaluate(top->branches[0]);
        for (unsigned int i = 1; i < top->branches.size(); i++)
        {
            result -= evaluate(top->branches[i]);
        }
    }
    else if (text == "*")
    {
        result = 1;
        for (Node *child : top->branches)
        {
            result *= evaluate(child);
        }
    }
    else if (text == "/")
    {
        result = evaluate(top->branches[0]);
        for (unsigned int i = 1; i < top->branches.size(); i++)
        {
            // Divide, but check for division by 0 error
            double divisor = evaluate(top->branches[i]);
            if (divisor == 0)
            {
                // Find the root of the tree and print infix version (the tree should still print in infix form when a runtime error occurs!)
                while (top->parent)
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
    else if (text == "=")
    {
        // for(int i = top->branches.size() - 2; i >= 0; i--)
        // First, check for assignee errors.
        for (unsigned int i = 0; i < top->branches.size() - 1; i++)
        {
            Token assignee = top->branches[i]->info;
            // invalid assignees are not variables.
            if (!assignee.isVariable())
            {
                // Note that OPERATORS are not thrown, the parentheses before them are.
                // if the first child was bad, it is thrown.
                if (i == 0)
                {
                    if (assignee.isOperator())
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
                    if (lastChild.isOperator())
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
        for (unsigned int i = 0; i < top->branches.size() - 1; i++)
        {
            variables[top->branches[i]->info.text] = result;
        }
    }
    else if (t.isNumber())
    {
        result = stod(text);
    }
    else if (t.isVariable())
    {
        // Test for undefined identifier error
        if (variables.find(text) == variables.end())
        {
            // Find the root of the tree and print infix version (the tree should still print in infix form when a runtime error occurs!)
            while (top->parent)
            {
                top = top->parent;
            }
            cout << printHelper(top, true) << endl;
            cout << "Runtime error: unknown identifier " << text << endl;
            exit(3);
        }
        else
        {
            result = variables[text];
        }
    }
    return (result);
}

void Parser::parseError(Token token)
{
    cout << "Unexpected token at line " << token.line << " column " << token.column << ": " << token.text << endl;
    exit(2);
}

Parser::~Parser()
{
    for (Node *root : roots)
    {
        clear(root);
    }
}

void Parser::clear(Node *top)
{
    if (!top)
    {
        return;
    }
    for (Node *child : top->branches)
    {
        clear(child);
    }
    delete top;
}

Token Parser::findParenthesisBefore(Token o)
{
    vector<Token> line = tokens[o.line - 1];
    for (unsigned int i = 0; i < line.size(); i++)
    {
        if (line[i].column == o.column)
        {
            return (line[i - 1]);
        }
    }
    // SHOULD NEVER BE REACHED
    return (o);
}
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <limits>
#include "token.h"
#include "lex.h"
#include "parse.h"
using namespace std;

Parser::Parser(vector<vector<Token>> inputFromLexer)
{
    for (auto expression : inputFromLexer)
    {
        // if (expression.back().text == "END")
        // {
        //     break;
        // }
        roots.push_back(constructAST(expression));
    }
    // Delete any vectors that are nullptr
    for (unsigned int i = 0; i < roots.size(); i++)
    {
        if (roots[i] == nullptr || roots.size() == 1)
        {
            roots.erase(roots.begin() + i);
            i--;
        }
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
    // CHECK FOR ALL UNEXPECTED TOKEN ERRORS (except for invalid assignees, which are caught in evaluate!)
    // The following function will print the error message on its own.
    // It returns true if there's an error detected.
    if (checkError(tokens) == true)
    {
        return nullptr;
    }
    // Remove the end token, which is no longer needed after checkError().
    tokens.pop_back();

    stack<Node *> nodeStack;
    stack<string> stringStack;
    Node *root = nullptr;
    Node *child1 = nullptr;
    Node *child2 = nullptr;
    if (tokens.size() == 0 || (tokens.size() == 1 && tokens[0].isEnd()))
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
                    nodeStack.push(root);
                }
            }
        }
        else if (tokens[i].text != ")")
        {
            while (!stringStack.empty() &&
                   stringStack.top() != "(" &&
                   ((tokens[i].text != "=" && getPrecedence(stringStack.top()) >= getPrecedence(tokens[i].text)) || (tokens[i].text == "=" && getPrecedence(stringStack.top()) > getPrecedence(tokens[i].text))))
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
                while (!stringStack.empty() && stringStack.top() != "("
                       //&& ((tokens[i].text != "=" && getPrecedence(stringStack.top()) >= getPrecedence(tokens[i].text)) || (tokens[i].text == "=" && getPrecedence(stringStack.top()) > getPrecedence(tokens[i].text)))
                )
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
        if (isnan(finalValue))
        {
            continue;
        }
        string finalInfix = printHelper(root, true);
        cout << finalInfix << endl;
        cout << finalValue << endl;
    }
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
                        return (std::numeric_limits<double>::quiet_NaN());
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

bool Parser::checkError(vector<Token> expression)
{
    int lastIndex = expression.size() - 2;
    Token theEnd = expression.back();
    if (!theEnd.isEnd())
    {
        cout << "ERROR: END TOKEN NOT PUSHED BACK TO EXPRESSION" << endl;
        cout << expression.back().text << endl;
        exit(4);
    }
    int parentheses = 0;
    for (int i = 0; i <= lastIndex; i++)
    {
        Token t = expression[i];

        // Operators should have two operands between them.
        // The left operand can be a RIGHT parenthesis or a number or an identifier.
        // The right operand can be a LEFT parenthesis or a number or an identifier.
        // If the operand ocurrs at the beginning or ending of the vector, that's also bad.
        if (t.isOperator())
        {
            if (i == 0 ||
                !(expression[i - 1].isNumber() || expression[i - 1].isVariable() || expression[i - 1].text == ")"))
            {
                parseError(t);
                return (true);
            }
            else if (i == lastIndex)
            {
                parseError(theEnd);
                return (true);
            }
            else if (!(expression[i + 1].isNumber() || expression[i + 1].isVariable() || expression[i + 1].text == "("))
            {
                // cout << "INVALID_OPERATOR" << endl;
                parseError(expression[i + 1]);
                return (true);
            }
        }
        // Parentheses should be balanced.
        // There should never be an empty set of two closed parentheses.
        // After an open parentheses, we must see a number or an identifier.
        if (t.text == "(")
        {
            parentheses++;
            if (i == lastIndex)
            {
                // cout << "INVALID_LEFT" << endl;
                parseError(theEnd);
                return (true);
            }
            if (!(expression[i + 1].isNumber() || expression[i + 1].isVariable()) || expression[i + 1].text == ")")
            {
                // cout << "INVALID_LEFT" << endl;
                parseError(expression[i + 1]);
                return (true);
            }
        }
        // Parentheses should be balanced.
        // Before a closed parentheses, we must see a number or an identifier.
        if (t.text == ")")
        {

            parentheses--;
            if (parentheses < 0) // This also covers the case where i == 0.
            {
                // cout << "INVALID_RIGHT" << endl;
                parseError(t);
                return (true);
            }
            if (!(expression[i - 1].isNumber() || expression[i - 1].isVariable()))
            {
                // cout << "INVALID_RIGHT" << endl;
                parseError(expression[i - 1]);
                return (true);
            }
        }
        // Numbers and identifiers should have operators or parentheses to either side.
        // The parentheses on either side must only be open (when to the left) or closed (to the right).
        //
        if (t.isNumber() || t.isVariable())
        {
            // Check left
            if (i != 0 && !(expression[i - 1].text == "(" || expression[i - 1].isOperator()))
            {
                // cout << "INVALID_NUM" << endl;
                parseError(t);
                return (true);
            }
            // Check right
            else if (i != lastIndex && !(expression[i + 1].text == ")" || expression[i + 1].isOperator()))
            {
                // cout << "INVALID_NUM" << endl;
                parseError(expression[i + 1]);
                return (true);
            }
        }
    }
    // At the very end of the expression, all the parentheses should be balanced.
    if (parentheses == 0)
    {
        return (false);
    }
    else
    {
        // cout << "UNBALANCED" << endl;
        parseError(theEnd);
        return (true);
    }
}

void Parser::parseError(Token token)
{
    token.line = 1;
    cout << "Unexpected token at line " << token.line << " column " << token.column << ": " << token.text << endl;
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
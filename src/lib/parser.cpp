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

int getPrecedence(string token) // Helper function for constructAST
{
    if (token == "=")
        return (1);
    else if (token == "<" || token == "<=" || token == ">" || token == ">=" || token == "==" || token == "!=")
        return (2);
    else if (token == "+" || token == "-" || token == "%")
        return (3);
    else if (token == "*" || token == "/")
        return (4);
    else
        return (5);
}

Parser::Node *Parser::constructAST(vector<Token> tokens)
{
    // CHECK FOR ALL UNEXPECTED TOKEN ERRORS
    // The following function will print the error message on its own.
    // It returns true if there's an error detected.
    if (checkError(tokens) == true)
    {
        return nullptr;
    }
    // Remove the end token, which is no longer needed after checkError().
    tokens.pop_back();

    stack<Node *> nodeStack;   // will contain child and root nodes before linking to each other
    stack<string> stringStack; // will contain operators and parentheses, used for determining order of tree
    Node *root = nullptr;
    Node *child1 = nullptr;
    Node *child2 = nullptr;
    if (tokens.size() == 0 || (tokens.size() == 1 && tokens[0].isEnd())) // handles case where expression is empty line
    {
        return nullptr;
    }
    for (unsigned int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].text == "(") // signifies beginning of subtree
        {
            stringStack.push("(");
        }
        else if (tokens[i].isOperand()) // numbers and variables are treated the same at a base level
        {
            nodeStack.push(new Node{Parser::Node{tokens[i], vector<Node *>(), nullptr}});
            if (i == tokens.size() - 1 && !stringStack.empty()) // checks if it is the end of the expression and there is still linking to be done
            {
                while (!stringStack.empty() && stringStack.top() != "(")
                {
                    string currentString = stringStack.top();
                    root = new Node{
                        Parser::Node{Token{0, (int)i, currentString}, vector<Node *>(), nullptr}};
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
        else if (tokens[i].text != ")") // operator case
        {
            while (!stringStack.empty() &&
                   stringStack.top() != "(" &&
                   ((tokens[i].text != "=" && getPrecedence(stringStack.top()) >= getPrecedence(tokens[i].text)) || (tokens[i].text == "=" && getPrecedence(stringStack.top()) > getPrecedence(tokens[i].text))))
            { // cases are treated differently based on if there is an = sign involved due to the right associativity of the = operator
                string currentString = stringStack.top();
                root = new Node{
                    Parser::Node{Token{0, (int)i + 1, currentString}, vector<Node *>(), nullptr}};
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
        else if (tokens[i].text == ")") // signifies end of subtree
        {
            while (!stringStack.empty() && stringStack.top() != "(")
            {
                string currentString = stringStack.top();
                root = new Node{
                    Parser::Node{Token{0, (int)i + 1, currentString}, vector<Node *>(), nullptr}};
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
            if (!stringStack.empty())
            { // checks if it is the end of the subtree and there is still linking to be done
                stringStack.pop();
                if (i == tokens.size() - 1)
                {
                    while (!stringStack.empty() && stringStack.top() != "(")
                    {
                        string currentString = stringStack.top();
                        root = new Node{
                            Parser::Node{Token{0, (int)i + 1, currentString}, vector<Node *>(), nullptr}};
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
        }
    }
    Node *finalRoot = nodeStack.top();
    return finalRoot;
}

void Parser::print() // Infix
{
    for (Node *root : roots)
    {
        for (auto s : variables)
        {
            provisional[s.first] = variables[s.first];
        }

        typedValue finalValue = evaluate(root);
        string finalInfix = printHelper(root, true);

        if(finalValue.type == ERROR)
        {
            cout << finalInfix << endl;
            cout << "Runtime error: invalid operand type." << endl;
            continue;
        }

        for (auto s : provisional)
        {
            variables[s.first] = provisional[s.first];
        }
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

typedValue Parser::evaluate(Node *top)
{
    typedValue result;
    result.setType(DOUBLE);
    result.data.doubleValue = 0;
    if (!top)
    {
        return result;
    }
    Token t = top->info;
    string text = top->info.text;
    // MISMATCH: DIFFERENT OPERANDS
    if((t.text != "=") && ((t.isOperator() && (!(t.text == "==" || t.text == "!=") 
    && (evaluate(top->branches[0]).type) != evaluate(top->branches[1]).type))
    || (t.takesBoolsOnly() && ((evaluate(top->branches[0]).type) == DOUBLE || evaluate(top->branches[1]).type == DOUBLE))
    || (t.takesDoublesOnly() && ((evaluate(top->branches[0]).type) == BOOLEAN || evaluate(top->branches[1]).type == BOOLEAN))) )
    {
        result.type = ERROR;
        return(result);
    }
    else if (text == "+")
    {
        for (Node *child : top->branches)
        {
            result.data.doubleValue += (evaluate(child)).data.doubleValue;
        }
    }
    else if (text == "-")
    {
        result.data.doubleValue = evaluate(top->branches[0]).data.doubleValue;
        for (unsigned int i = 1; i < top->branches.size(); i++)
        {
            result.data.doubleValue -= evaluate(top->branches[i]).data.doubleValue;
        }
    }
    else if (text == "%")
    {
        result.data.doubleValue = evaluate(top->branches[0]).data.doubleValue;
        for (unsigned int i = 1 ; i < top->branches.size() ; i++)
        {
            // Do modulus, but check for division by 0
            double d2 = evaluate(top->branches[i]).data.doubleValue;
            if (d2 == 0)
            {
                while (top->parent) top = top -> parent;
                cout << printHelper(top, true) << endl;
                cout << "Runtime error: division by zero." << endl;
                result.data.doubleValue = std::numeric_limits<double>::quiet_NaN();
                return (result);
            }
            //result = result % d2;
            result.data.doubleValue = result.data.doubleValue - d2 * std::floor(result.data.doubleValue / d2);
        }
    }
    else if (text == "*")
    {
        result.data.doubleValue = 1;
        for (Node *child : top->branches)
        {
            result.data.doubleValue *= evaluate(child).data.doubleValue;
        }
    }
    else if (text == "/")
    {
        result = evaluate(top->branches[0]);
        for (unsigned int i = 1; i < top->branches.size(); i++)
        {
            // Divide, but check for division by 0 error
            double divisor = evaluate(top->branches[i]).data.doubleValue;
            if (divisor == 0)
            {
                // Find the root of the tree and print infix version (the tree should still print in infix form when a runtime error occurs!)
                while (top->parent)
                {
                    top = top->parent;
                }
                cout << printHelper(top, true) << endl;
                cout << "Runtime error: division by zero." << endl;
                result.data.doubleValue = std::numeric_limits<double>::quiet_NaN();
                return (result);
            }
            result.data.doubleValue /= divisor;
        }
    }
    // The assignment operator is right-associative, so it evaluates the last (rightmost) child
    // of the operator in the AST to figure out what to assign these variables to.
    // doASSIGNMENT
    else if (text == "=")
    {
        // There are no assignee errors at this point (they were caught in checkError), assign all the operands to the value of the rightmost expression.
        result = evaluate(top->branches[1]);
        string key = top->branches[0]->info.text;
        provisional[key] = result;
    }
    else if (t.isNumber())
    {
        result = (top->info.getValue());
    }
    else if (t.isVariable())
    {
        // Test for undefined identifier error
        if (provisional.find(text) == provisional.end())
        {
            // Find the root of the tree and print infix version (the tree should still print in infix form when a runtime error occurs!)
            while (top->parent)
            {
                top = top->parent;
            }
            cout << printHelper(top, true) << endl;
            cout << "Runtime error: unknown identifier " << text << endl;
            result.data.doubleValue = std::numeric_limits<double>::quiet_NaN();
            return (result);
        }
        else
        {
            result = provisional[text];
        }
    }
    else if(t.isBoolean())
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = t.getValue().data.booleanValue;
    }
    // ALL LOGIC OPERATORS
    else if (text == "<")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue < evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == ">")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue > evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == ">=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue >= evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == "<=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue <= evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == "==")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]) == evaluate(top->branches[1]));
    }
    else if (text == "|")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.booleanValue || evaluate(top->branches[1]).data.booleanValue);
    }
    else if (text == "&")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.booleanValue && evaluate(top->branches[1]).data.booleanValue);
    }
    else if (text == "^")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.booleanValue != evaluate(top->branches[1]).data.booleanValue);
    }
    return (result);
}

bool Parser::checkError(vector<Token> expression) // runs before we try evaluating
{
    int lastIndex = expression.size() - 2;
    Token theEnd = expression.back();
    if (!theEnd.isEnd()) // make sure formatting of END is correct
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
        if (t.isOperator() || t.isOrderComparison())
        {
            if (i == 0 ||
                !(expression[i - 1].isOperand() || expression[i - 1].text == ")"))
            {
                // cout << "INVALID OPERATOR: CASE 1" << endl;
                parseError(t);
                return (true);
            }
            else if (i == lastIndex)
            {
                // cout << "INVALID OPERATOR: CASE 2" << endl;
                parseError(theEnd);
                return (true);
            }
            else if (!(expression[i + 1].isOperand() || expression[i + 1].text == "("))
            {
                // cout << "INVALID OPERATOR: CASE 3" << endl;
                parseError(expression[i + 1]);
                return (true);
            }
        }
        // The left of every equals sign should only be an identifier.
        if (t.text == "=")
        {
            if (!(expression[i - 1].isVariable()))
            {
                parseError(t);
                return (true);
            }
        }
        // Parentheses should be balanced.
        // There should never be an empty set of two closed parentheses.
        // After an open parentheses, we must see a number or an identifier or another open parenthesis.
        else if (t.text == "(")
        {
            parentheses++;
            if (i == lastIndex)
            {
                parseError(theEnd);
                return (true);
            }
            if (!(expression[i + 1].isOperand() || expression[i + 1].text == "(") || expression[i + 1].text == ")")
            {
                parseError(expression[i + 1]);
                return (true);
            }
        }
        // Parentheses should be balanced.
        // Before a closed parentheses, we must see a number or an identifier or another closed parenthesis.
        else if (t.text == ")")
        {

            parentheses--;
            if (parentheses < 0) // This also covers the case where i == 0.
            {
                parseError(t);
                return (true);
            }
            if (!(expression[i - 1].isOperand() || expression[i - 1].text == ")"))
            {
                parseError(expression[i - 1]);
                return (true);
            }
        }
        // Numbers and identifiers should have operators or parentheses to either side.
        // The parentheses on either side must only be open (when to the left) or closed (to the right).
        else if (t.isOperand())
        {
            // Check left
            if (i != 0 && !(expression[i - 1].text == "(" || expression[i - 1].isOperator()))
            {
                parseError(t);
                return (true);
            }
            // Check right
            else if (i != lastIndex && !(expression[i + 1].text == ")" || expression[i + 1].isOperator()))
            {
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
#include <iostream>
#include <string>
#include <vector>
#include "token.h"
#include "lex.h"
#include "parser_infix.h"
using namespace std;

int getPrecedence(string token)
{
    if (token == "=") return(1);
    else if (token == "+" || token == "-") return(2);
    else if (token == "*" || token == "/") return(3);
    else return(4); 
}

vector<Token> infixToPostfix(vector<Token> tokens) {
    vector<Token> output;
    vector<Token> operators;
    for (Token token : tokens) {
        if (getPrecedence(token.text) == 4) {
            output.push_back(token);
        } else if (getPrecedence(token.text) == 3 || getPrecedence(token.text) == 2) {
            while (!operators.empty() && (getPrecedence(operators.back().text) == 3 || getPrecedence(operators.back().text) == 2) &&
                   getPrecedence(operators.back().text) >= getPrecedence(token.text)) {
                output.push_back(operators.back());
                operators.pop_back();
            }
            operators.push_back(token);
        } else if (token.text == "(") {
            operators.push_back(token);
        } else if (token.text == ")") {
            while (!operators.empty() && operators.back().text != "(") {
                output.push_back(operators.back());
                operators.pop_back();
            }
            if (!operators.empty() && operators.back().text == "(") {
                operators.pop_back();
            }
        } else if (token.text == "=") {
            operators.push_back(token);
        }
    }
    while (!operators.empty()) {
        output.push_back(operators.back());
        operators.pop_back();
    }
    return output;
}


vector<Token> addParentheses(vector<Token> tokens) {
    return tokens; //TBD
}



ParserInfix::ParserInfix(vector<vector<Token>> inVectors) 
{
    vectors = inVectors;
    for (vector<Token> tokens : inVectors)
    {
        // The expression is just a number
        if(tokens.size() == 1 && tokens[0].isNumber())
        {
            roots.push_back(new Node{ParserInfix::Node{tokens[0], vector<Node*>(), nullptr}});
        }

        // ERROR - The expression is empty 
        else if(tokens.size() < 1)
        {
            if(tokens[0].line != 1) 
                tokens[0].line++;
            parseError(tokens[0]);
        }
// 3 + 4
        else // The expression is ok
        {


            

            // just testing something:
            //vector<Token> postfix = addParentheses(infixToPostfix(tokens));
            //for (Token token : postfix) cout << token.text << " ";




/**
            Node* root = new Node{ParserInfix::Node{tokens[1], vector<Node*>(), nullptr}};
            root->branches.push_back(new Node{ParserInfix::Node{tokens[0], vector<Node*>(), root}});
            Node* currNode = root;

            for (unsigned int i = 2 ; i < tokens.size() ; i++) 
            {
                Token token = tokens[i];
                string data = token.text;

                if (getPrecedence(data) > getPrecedence(currNode->info.text))
                {
                    Node* newNode = new Node{ParserInfix::Node{token, vector<Node*>(), currNode}};
                    currNode->branches.push_back(newNode);
                    if (getPrecedence(data) < 4) currNode = newNode;
                }
                else if (getPrecedence(data) < getPrecedence(currNode->info.text))
                {
                    vector<Node*> branches;
                    branches.push_back(currNode);
                    Node* newCurrNode = new Node(ParserInfix::Node{token, branches, nullptr});
                    currNode->parent = newCurrNode;
                    if (currNode == root) root = newCurrNode;
                    currNode = newCurrNode;
                    
                }
            }
            roots.push_back(root);
            */
        }

    }
}





vector<double> ParserInfix::evaluate()
{
    vector<double> results;
    for (Node* root : roots)
        results.push_back(evaluateHelper(root));
    return(results);
}

double ParserInfix::evaluateHelper(Node * top)
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
        result = stod(text);
    }
    return(result);
}   


ParserInfix::~ParserInfix()
{
    for (Node* root : roots)
        clear(root);
}

void ParserInfix::clear(Node* top)
{
    for(Node * child : top->branches)
        {
            clear(child);
        }
    delete top;
}

void ParserInfix::parseError(Token token)
{
    cout << "Unexpected token at line "<< token.line << " column " << token.column << ": " << token.text << endl;
    exit(2);
}
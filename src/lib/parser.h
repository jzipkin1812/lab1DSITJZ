#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <string>
#include <vector>
#include "token.h"
using namespace std;

class Parser
{
public:
    double evaluate();
    Parser(vector<Token> inTokens);
    void print();
    ~Parser();

private:
    vector<Token> tokens;
    
    struct Node
    {
        Token info;
        vector<Node *> branches;
        Node * parent;
    };

    Node *root = nullptr;
    string printHelper(Node * top, bool lastChild);
    double evaluateHelper(Node * top);
    void parseError(Token token);
   
    void clear(Node * top);
};

#endif

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

private:
    vector<Token> tokens;
    
    struct Node
    {
        Token info;
        vector<Node *> branches;
        Node * parent;
        // useful constructor:
        // Node(Token tk)
        // {
        //     info = tk;
        // }
        // Node()
        // {
        //     info = Token{0, 0, ""};
        // }
    };

    Node *root;
    string printHelper(Node * top, bool lastChild);
    double evaluateHelper(Node * top);
    void parseError(Token token);
};

#endif
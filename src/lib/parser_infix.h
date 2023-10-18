#ifndef PARSER_INFIX_H
#define PARSER_INFIX_H

#include <iostream>
#include <string>
#include <vector>
#include "token.h"
using namespace std;

class ParserInfix
{
public:
    double evaluate();
    ParserInfix(vector<vector<Token>> inVectors);
    void print();
    ~ParserInfix();

private:
    vector<vector<Token>> vectors;
    
    struct Node
    {
        Token info;
        vector<Node *> branches;
        Node * parent;
    };

    vector<Node*> roots;
    string printHelper(Node * top, bool lastChild);
    double evaluateHelper(Node * top);
    void parseError(Token token);
   
    void clear(Node * top);
};

#endif

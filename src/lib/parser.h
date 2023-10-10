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
        Parser(vector<Token> tokens);
        void print();
    
    private:
        
        struct Node 
        {
            Token info;
            vector<Node*> branches;
            // useful constructor:
            Token tk = Token{0, 0, ""};
            // Node(tk) :  info(tk), branches() {}
        };

        Node * root;
};

#endif
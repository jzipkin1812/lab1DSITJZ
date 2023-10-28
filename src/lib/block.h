#ifndef BLOCK_H
#define BLOCK_H

#include "token.h"
#include "node.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Block
{
    public:
        string statementType; // Expression, if, else, while, etc
        Node * condition;
        vector<Block *> nestedStatements;
        Node * root; // Only useful for subclass
        Block(Node * AST)
        {
            statementType = "";
            condition = nullptr;
            nestedStatements = vector<Block *>();
            root = AST;
        }

    private:    
        

};

#endif
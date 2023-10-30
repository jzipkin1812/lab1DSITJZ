#ifndef BLOCK_H
#define BLOCK_H

#include "token.h"
#include "node.h"
#include "typedValue.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class Block
{
    public:
        string statementType; // Expression, if, else, while, etc
        Node * condition; // Used for IF, ELSE, WHILE only
        vector<Block> nestedStatements; // Used for IF, ELSE, WHILE only
        Node * root; // Used for expressions and print only
        Block * parent; // Used for nested blocks only
        Block()
        {
            statementType = "";
            condition = nullptr;
            nestedStatements = vector<Block>();
            root = nullptr;
        }
        Block(Node * AST)
        {
            statementType = "expression";
            condition = nullptr;
            nestedStatements = vector<Block>();
            root = AST;
        }
        Block(string type, Node * AST, Block * outer = nullptr)
        {
            parent = outer;
            statementType = type;
            if(type == "print")
            {
                condition = nullptr;
                root = AST;
            }
            else
            {
                condition = AST;
                root = nullptr;
            }
            nestedStatements = vector<Block>();
        }
        void nest(Block b)
        {
            nestedStatements.push_back(b);
        }

    private:    
        

};

#endif
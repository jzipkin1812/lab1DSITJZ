#ifndef BLOCK_H
#define BLOCK_H

#include "token.h"
#include "node.h"
#include <vector>
#include <string>
#include <map>
#include <iostream>
using namespace std;
 
// Types of blocks
// EXPRESSION:
// statementType = "expression"
// root = constructAST(all the tokens in the expression)
// everything else is a nullptr or empty

// WHILE:
// IF:
// statementType = "if" or "while"
// condition = constructAST(all the tokens in the condition of the if)
// nestedStatements = all the blocks inside the brackets
// root = nullptr

// DEF:
// statementType = "def"
// nestedStatements = everything in there
// arguments = list of all the arguments in order with their names
// root = nullptr

// PRINT, RETURN:
// statementType = "print" or "return"
// root = constructAST(what we want to print or return)
// everything else null or empty

// ELSE:
// Belongs to the if statement via * elseStatement pointer

class Block
{
    public:
        string statementType; // Expression, if, else, while, or print
        string functionName; // Function's name in a def statement only
        Node * condition; // Used for IF, ELSE, WHILE only
        vector<Block> nestedStatements; // Used for IF, ELSE, WHILE, DEF only
        vector<string> argumentNames; // Used for DEF only
        map<string, typedValue> capturedVariables; // Used for DEF only
        Node * root; // Used for expressions and print only
        Block * parent; // Used for nested blocks only
        Block * elseStatement; // Used for if statements only
        int closingLine; // used for if statements only to track when exactly they end for blocking
        Block()
        {
            statementType = "";
            condition = nullptr;
            nestedStatements = vector<Block>();
            root = nullptr;
            elseStatement = nullptr;
        }
        Block(Node * AST)
        {
            statementType = "expression";
            condition = nullptr;
            nestedStatements = vector<Block>();
            root = AST;
            elseStatement = nullptr;
        }
        Block(string type, Node * AST, Block * outer = nullptr)
        {
            parent = outer;
            statementType = type;
            if(type == "print" || type == "return")
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
            elseStatement = nullptr;
        }
        void nest(Block b)
        {
            nestedStatements.push_back(b);
        }

    private:    
};

class Func: public Block
{
    public:
        map<string, typedValue> variables;
        unsigned int argc;
        Func()
        {
            argc = 0;
            Block();
        }
        Func(Block information, map<string, typedValue> captured)
        {
            functionName = information.functionName;
            variables = captured;
            argumentNames = information.argumentNames;
            argc = argumentNames.size();
            nestedStatements = information.nestedStatements;
        }
};

#endif
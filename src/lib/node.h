#ifndef NODE_H
#define NODE_H

#include "token.h"
#include <vector>


struct Node
{
    Token info;
    vector<Node *> branches;
    Node *parent;
    bool isFunctionCall = false; // Default-set to false. You must change this only if it's a function CALL
    bool isArrayAssignment = false;
    bool isArrayLiteral = false;
    bool isArrayLookup = false;
};

#endif


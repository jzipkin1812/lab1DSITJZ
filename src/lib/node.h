#ifndef NODE_H
#define NODE_H

struct Node
{
    Token info;
    vector<Node *> branches;
    Node *parent;
    bool isFunctionCall = false; // Default-set to false. You must change this only if it's a function
};

#endif
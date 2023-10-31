#ifndef NODE_H
#define NODE_H

struct Node
{
    Token info;
    vector<Node *> branches;
    Node *parent;
};

#endif
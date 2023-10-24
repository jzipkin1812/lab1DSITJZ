#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "token.h"
using namespace std;

class Parser
{
public:
    Parser(vector<vector<Token>> inputFromLexer);
    void print();
    ~Parser();

private:
    vector<vector<Token>> tokens;
    // vector<Token> singleTokens;
    // vector<Token> oneExpression();

    struct Node
    {
        Token info;
        vector<Node *> branches;
        Node *parent;
    };

    vector<Node *> roots;
    string printHelper(Node *top, bool lastChild);
    double evaluate(Node *top);
    bool checkError(vector<Token> expression);
    void parseError(Token token);
    void clear(Node *top);
    Node *constructAST(vector<Token> tokens);
    Token findParenthesisBefore(Token o);
    // The map maps variables to their values
    map<string, double> variables;
    // This map is used to set the variables before checking for runtime errors. If there are no runtime errors by the end of evaluation, provisional is transfered to variables.
    map<string, double> provisional;
};

#endif

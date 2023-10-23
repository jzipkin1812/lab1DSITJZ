#ifndef PARSER_INFIX_H
#define PARSER_INFIX_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "token.h"
using namespace std;

class ParserInfix
{
public:
    ParserInfix(vector<vector<Token>> inputFromLexer);
    void print();
    ~ParserInfix();

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
    void parseError(Token token);
    void clear(Node *top);
    Node *constructAST(vector<Token> tokens);
    Token findParenthesisBefore(Token o);
    // The map maps variables to their values
    map<string, double> variables;
};

#endif

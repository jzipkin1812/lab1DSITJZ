#ifndef PARSE_H
#define PARSE_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "token.h"
#include "block.h"
#include "typedValue.h"
#include "node.h"

using namespace std;

class Parser
{
public:
    Parser(vector<vector<Token>> inputFromLexer, bool statements = false);
    void print();
    void format();
    void execute();
    ~Parser();

private:
    bool exitImmediately;
    bool allowStatements;
    vector<vector<Token>> tokens;
    vector<Block> blocks;
    string printHelper(Node * top, bool lastChild);
    typedValue evaluate(Node *top);
    stringstream finalOutput;
    bool checkError(vector<Token> expression, int line = 0);
    void parseError(Token token, int line = 0);
    void clearNode(Node *top);
    void clearBlock(Block b);
    Node *constructAST(vector<Token> tokens, int line = 0);
    void executeHelper(Block b);
    void formatHelper(Block b, unsigned int indents = 0);
    vector<stringstream> outputPerExpression;
    // The map maps variables to their values
    map<string, typedValue> variables;
    // This map is used to set the variables before checking for runtime errors. If there are no runtime errors by the end of evaluation, provisional is transfered to variables.
    map<string, typedValue> provisional;
};



#endif

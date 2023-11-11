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
    // The map maps variables to their values
    map<string, typedValue> variables;
    // This map is used to set the variables before checking for runtime errors. If there are no runtime errors by the end of evaluation, provisional is transfered to variables.
    map<string, typedValue> provisional;
    vector<vector<Token>> tokens;
    vector<Block> blocks;
    string printHelper(Node * top, bool lastChild);
    typedValue evaluate(Node *top, map<string, typedValue>& scopeMap);
    stringstream finalOutput;
    bool checkError(vector<Token> expression, int line = 0, bool requireSemicolons = false);
    void parseError(Token token, int line = 0);
    void clearNode(Node *top);
    void clearBlock(Block b);
    Node *constructAST(vector<Token> tokens, int line = 0, bool requireSemicolons = false, bool ignoreErrors = false);
    typedValue executeHelper(Block b, map<string, typedValue>& scope, bool allowReturns = false);
    void formatHelper(Block b, unsigned int indents = 0);
    bool containsClose(vector<Token> line);
    bool containsOpen(vector<Token> line);
    int getPrecedence(string token);
    unsigned int chainEndIndex(vector<vector<Token>> program, unsigned int lineNum);
    typedValue callFunction(Func givenFunction, vector<typedValue> arguments);
    unsigned int nextClose(vector<vector<Token>> program, unsigned int lineNum);
    vector<vector<Token>> cleanInput(vector<vector<Token>> inputFromLexer);
    vector<stringstream> outputPerExpression;    
    vector<Func *> globalFunctions; // used for memory clearing at the end of the program.
};



#endif

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <limits>
#include <sstream>
#include "token.h"
#include "node.h"
#include "block.h"
#include "lex.h"
#include "parse.h"
using namespace std;


int Parser::getPrecedence(string token) // Helper function for constructAST
{
    if (token == "=")
        return (1);
    else if (token == "|")
        return (2);
    else if (token == "^")
        return (3);
    else if (token == "&")
        return (4);
    else if (token == "==" || token == "!=")
        return (5);
    else if (token == "<" || token == "<=" || token == ">" || token == ">=")
        return (6);
    else if (token == "+" || token == "-")
        return (7);
    else if (token == "*" || token == "/"  || token == "%")
        return (8);
    else
        return (9);
}

string Parser::printHelper(Node *top, bool lastChild)
{
    string finalText = "";
    if (!top)
        return finalText;
    bool last;
    if (top->info.isOperator())
    {
        finalText += "(";
        for (unsigned int i = 0; i < top->branches.size(); i++)
        {
            last = (i == top->branches.size() - 1);
            finalText += printHelper(top->branches[i], last);
        }
        finalText += ")";
        if (!lastChild)
        {
            finalText += " " + top->parent->info.text + " ";
        }
    }
    else if (top->info.isOperand() && top->branches.size() > 0)
    {
        finalText += top->info.text + "(";
        for(unsigned int j = 0; j < top->branches.size(); j++)
        {
            Node * param = top->branches[j];
            if(j < top->branches.size() - 1) finalText += printHelper(param, true) + ", ";
            else finalText += printHelper(param, true) + ")";
            
        }
    }
    else
    {
        string converted = top->info.text;
        // Formatting removes trailing 0s
        if (converted != "0" && converted.find('.') != string::npos)
        {
            converted.erase(converted.find_last_not_of('0') + 1, std::string::npos);
            converted.erase(converted.find_last_not_of('.') + 1, std::string::npos);
        }

        finalText += converted;

        if (!lastChild)
        {
            // Space, parent operator, space
            finalText += " " + top->parent->info.text + " ";
        }
    }
    return (finalText);
}

Parser::~Parser()
{
    for (Block block : blocks)
    {
        clearBlock(block);
    }
}

void Parser::clearNode(Node *top)
{
    if (!top)
    {
        return;
    }
    for (Node *child : top->branches)
    {
        clearNode(child);
    }
    delete top;
}

void Parser::clearBlock(Block b)
{
    clearNode(b.condition);
    clearNode(b.root);
    if(b.elseStatement)
    {
        clearBlock(*(b.elseStatement));
        delete b.elseStatement;
    }
    for(Block g : b.nestedStatements)
    {
        clearBlock(g);
    }
}

void Parser::execute()
{
    for(Block b : blocks)
    {
        executeHelper(b, provisional, false);
    }
    // for(auto variablePair : provisional)
    // {
    //     if(variablePair.second.type != FUNCTION) cout << variablePair.first << " : " << variablePair.second << endl;
    //     else
    //     {
    //         typedValue var = variablePair.second;

    //         Func * converted = reinterpret_cast<Func*>(var.data.functionValue);
    //         cout << variablePair.first << " is a function with this information:\n";
    //         cout << converted->argc << " arguments, " << converted->nestedStatements.size() << " nested blocks, and the name " << converted->functionName << endl;
    //     }
        
    // }
}

bool Parser::containsOpen(vector<Token> line)
{
    for(Token t : line)
    {
        if(t.text == "{") return true;
    }
    return (false);
}

bool Parser::containsClose(vector<Token> line)
{
    for(Token t : line)
    {
        if(t.text == "}") return true;
    }
    return (false);
}


unsigned int Parser::nextClose(vector<vector<Token>> program, unsigned int lineNum)
{
    int brackets = 0;
    for(; lineNum < program.size(); lineNum++)
    {
        vector<Token> line = program[lineNum];
        if(containsOpen(line))
        {
            brackets++;
        }
        if(containsClose(line))
        {
            brackets--;
            if(brackets == 0)
            {
                return(lineNum);
            }
        }
    }
    return(0);
}

void Parser::print() // Infix, no statements, no semicolons
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        Block oneBlock = blocks[i];
        if(oneBlock.root == nullptr)
        {
            continue;
        }
        for (auto s : variables)
        {
            provisional[s.first] = variables[s.first];
        }

        typedValue finalValue = evaluate(oneBlock.root, provisional);
        // cout << finalValue.type << "\n";
        string finalInfix = printHelper(oneBlock.root, true);
        outputPerExpression[i] << finalInfix << "\n";
        if(finalValue.isError())
        {
            outputPerExpression[i] << finalValue.outputError(false);
        }
        else
        {
            for (auto s : provisional)
            {
                variables[s.first] = provisional[s.first];
            }
            outputPerExpression[i] << finalValue << "\n";
        }
    }
    for(unsigned int i = 0; i < outputPerExpression.size(); i++)
    {
        cout << outputPerExpression[i].str();
    }
}
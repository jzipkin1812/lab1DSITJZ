#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cmath>
#include <limits>
#include <stack>
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
    else if (token == "*" || token == "/" || token == "%")
        return (8);
    else
        return (9);
}

string Parser::printHelper(Node *top, bool lastChild)
{
    cout << "???????" << endl;
    // cout << "top = " << top->info.text << endl;
    string finalText = "";
    if (!top)
        return finalText;
    bool last;
    if (top->info.text == "[") // array
    {
        //cout << "last = " << lastChild << endl;
        finalText += "[";
        vector<Node *> elements = top->branches;
        for (unsigned int i = 0; i < elements.size(); i++)
        {
            finalText += printHelper(elements[i], true);
            if (i < elements.size() - 1)
                finalText += ", ";
        }
        finalText += "]";
        if (!lastChild && top->parent && top->parent->info.isOperator())
        {
            // Space, parent operator, space
            finalText += " " + top->parent->info.text + " ";
        }
    }
    else if (top->info.text == "[.]")
    {
        finalText += printHelper(top->branches[0], true);
        finalText += "[";
        finalText += printHelper(top->branches[1], true);
        finalText += "]";
        if (!lastChild && top->parent && top->parent->info.isOperator())
        {
            // Space, parent operator, space
            finalText += " " + top->parent->info.text + " ";
        }
    }
    else if (top->info.isOperator())
    {
        finalText += "(";
        //cout << "top = " << top->info.text << " and size = " << top->branches.size() << endl;
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
    else if (top->info.isOperand() && top->isFunctionCall)
    {
        finalText += top->info.text + "(";
        for (unsigned int j = 0; j < top->branches.size(); j++)
        {
            Node *param = top->branches[j];
            if (j < top->branches.size() - 1)
                finalText += printHelper(param, true) + ", ";
            else
                finalText += printHelper(param, true);
        }
        finalText += ")";
    }
    // else if (top->info.isVariable() && top->branches.size() == 1 && top->branches[0]->info.text == "[")
    // {
    //     finalText += top->info.text;
    //     finalText += printHelper(top->branches[0], false);
    // }
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

        if (top->branches.size() == 1 && top->branches[0]->info.text == "[")
        {
            finalText += printHelper(top->branches[0], false);
        }

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
    for (Func *funcPtr : globalFunctions)
    {
        delete funcPtr;
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
    if (b.elseStatement)
    {
        clearBlock(*(b.elseStatement));
        delete b.elseStatement;
    }
    for (Block g : b.nestedStatements)
    {
        clearBlock(g);
    }
}

void Parser::execute()
{
    for (Block b : blocks)
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
    for (Token t : line)
    {
        if (t.text == "{")
            return true;
    }
    return (false);
}

bool Parser::containsClose(vector<Token> line)
{
    for (Token t : line)
    {
        if (t.text == "}")
            return true;
    }
    return (false);
}

unsigned int Parser::nextClose(vector<vector<Token>> program, unsigned int lineNum)
{
    int brackets = 0;
    for (; lineNum < program.size(); lineNum++)
    {
        vector<Token> line = program[lineNum];
        if (containsOpen(line))
        {
            brackets++;
        }
        if (containsClose(line))
        {
            brackets--;
            if (brackets == 0)
            {
                return (lineNum);
            }
        }
    }
    return (0);
}

unsigned int Parser::chainEndIndex(vector<vector<Token>> program, unsigned int lineNum)
{
    int brackets = 0;
    for (; lineNum < program.size(); lineNum++)
    {
        vector<Token> line = program[lineNum];
        Token first = *line.begin();
        if (first.text == "else" || (lineNum < program.size() - 1 && program[lineNum + 1][0].text == "else"))
        {
            continue;
        }
        if (containsOpen(line))
        {
            brackets++;
        }
        if (containsClose(line))
        {
            brackets--;
            if (brackets == 0)
            {
                return (lineNum);
            }
        }
    }
    return (0);
}
vector<vector<Token>> Parser::cleanInput(vector<vector<Token>> inputFromLexer)
{
    vector<Token> &lastLine = inputFromLexer[inputFromLexer.size() - 1];
    vector<Token> line;

    stack<unsigned int> closingLines;

    if ((lastLine.size() >= 2) && lastLine[lastLine.size() - 2].isEnd())
    {
        lastLine.erase(lastLine.end() - 1);
    }
    // Get rid of any pure whitespace lines
    for (unsigned int i = 0; i < inputFromLexer.size(); i++)
    {
        if ((*inputFromLexer[i].begin()).isEnd())
        {
            inputFromLexer.erase(inputFromLexer.begin() + i);
            i--;
        }
    }
    // Convert any else-if statements to else THEN if statements
    for (unsigned int i = 0; i < inputFromLexer.size(); i++)
    {
        line = inputFromLexer[i];
        Token beginning = line[0];
        Token second = line[1];
        // cout << "\t " << i << endl;
        if (beginning.text == "if")
        {
            // cout << "closing line: ";
            closingLines.push(chainEndIndex(inputFromLexer, i));
            // cout << closingLines.top() << endl;
        }
        else if (beginning.text == "else" && second.text == "if")
        {
            // cout << "elif at " << i << flush;
            vector<Token> ifStatement = vector<Token>(line.begin() + 1, line.end());
            // cout << " created if statement " << flush;
            inputFromLexer[i].erase(inputFromLexer[i].begin() + 1, inputFromLexer[i].end());
            // cout << " erased " << flush;
            inputFromLexer[i].push_back(Token(0, 0, "{"));
            inputFromLexer.insert(inputFromLexer.begin() + i + 1, ifStatement); // cout << " inserted back in " << flush;
            vector<Token> oneBracket = {Token(0, 0, "}"), Token(0, 0, "END")};
            inputFromLexer.insert(inputFromLexer.begin() + closingLines.top() + 1, oneBracket);
            // cout << " bracketed " << flush;
            // cout << " is DONE" << endl;
        }
        else
        {
            if (!closingLines.empty() && i == closingLines.top())
                closingLines.pop();
        }
    }
    // for(auto myLine : inputFromLexer)
    // {
    //     for(auto t : myLine)
    //     {
    //         cout << t.text << " ";
    //     }
    //     cout << endl;
    // }
    return (inputFromLexer);
}

vector<vector<Token>> Parser::separateLines(vector<vector<Token>> input)
{
    vector<vector<Token>> result;
    vector<Token> dividedLine;
    for (auto line : input)
    {
        for (Token t : line)
        {
            dividedLine.push_back(t);
            if (t.isBrace() || t.isSemicolon()) // these are the only way to end lines.
            {
                result.push_back(dividedLine);
                dividedLine.clear();
            }
        }
    }
    // Once more at the end, in case of a parse error where semicolons/braces were not used at all.
    if (dividedLine.size() > 0)
    {
        result.push_back(dividedLine);
        dividedLine.clear();
    }

    // Now it's time to make sure all our END tokens are normal. End tokens in the middle of the expression should be removed,
    // and lines without end tokens should be given one.
    for (unsigned int line = 0; line < result.size(); line++)
    {
        for (unsigned int col = 0; col < result[line].size(); col++)
        {
            Token t = result[line][col];
            if (t.isEnd() && col < result[line].size() - 1) // Excess END!
            {
                result[line].erase(result[line].begin() + col);
                col--;
            }
            else if (!t.isEnd() && col == result[line].size() - 1) // Oops, we need another END!
            {
                result[line].push_back(Token(line, t.column + 1, "END"));
            }
        }
    }
    return (result);
}

void Parser::print() // Infix, no statements, no semicolons
{
    for (unsigned int i = 0; i < blocks.size(); i++)
    {
        Block oneBlock = blocks[i];
        if (oneBlock.root == nullptr)
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
        if (finalValue.isError())
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
    for (unsigned int i = 0; i < outputPerExpression.size(); i++)
    {
        cout << outputPerExpression[i].str();
    }
}
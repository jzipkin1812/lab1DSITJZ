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

Parser::Parser(vector<vector<Token>> inputFromLexer, bool statements)
{
    exitImmediately = allowStatements = statements;
    // Simple process of 1 expression per line used when statements are not present.
    if(!statements)
    {
        for (unsigned int line = 0; line < inputFromLexer.size(); line++)
        {
            outputPerExpression.push_back(stringstream());
            blocks.push_back(Block(constructAST(inputFromLexer[line], line)));
        }
    }

    // A more complex process of "blocking" occurs when statements are present.
    else
    {
        vector<Token> &lastLine = inputFromLexer[inputFromLexer.size() - 1];
        if((lastLine.size() >= 2) && lastLine[lastLine.size() - 2].isEnd())
        {
            lastLine.erase(lastLine.end() - 1);
        }
        // The "target" points to the block vector that we should currently be stuffing things into.
        // It starts as the parser's block vector, but if we encounter while/if/else statements,
        // we'll have to change it so that future blocks are added to what is nested within.
        Block * targetParent = nullptr;
        vector<Block> * target = &(blocks);
        vector<Token> line;
        Token beginning(0, 0, "");
        for(unsigned int i = 0; i < inputFromLexer.size(); i++)
        {
            line = inputFromLexer[i];
            beginning = line[0];
            // cout << "Beginning of line " << i << " : " << beginning.text << endl;
            // cout << "--------LINE STARTING WITH " << beginning.text << endl;
            // Find the correct block vector to push things into. If parent is nullptr, that means we're not nested in anything and it should just be blocks.
            if(!targetParent)
            {
                target = &(blocks);
            }
            else
            {
                // cout << "Changing target to nestedStatements of target, which currently has size " << parent->nestedStatements.size() << endl;
                target = &(targetParent->nestedStatements);
            }

            // CASE 0: An end token.
            // Skip it.
            if(beginning.isEnd())
            {
                continue;
            }
            // CASE 1: An expression.
            // Construct an AST and push it to target.
            else if(!(beginning.isStatement() || beginning.isBrace()))
            {   
                // cout << "Beginning AST starting with: " << beginning.text << endl;
                (*target).push_back(Block(constructAST(line, i)));
                // cout << "Created an AST" << endl;
            }
            // CASE 2: A print statement.
            // Pop the print token. Construct an AST. Push a new block to target with this AST and type "print".
            // We don't need to track the parents of print statements. They don't nest.
            else if(beginning.text == "print")
            {
                line.erase(line.begin());
                (*target).push_back(Block("print", constructAST(line, i), targetParent));
                // cout << "After pushing the print, target's size is " << (*target).size() << endl;
            }
            // CASE 3: An if statement.
            // Pop the if token. Pop the brace. Construct an AST for the condition. 
            // Change "target" to this if statement's nestedStatements vector so that statements on future lines are added to it.
            else if(beginning.text == "if")
            {
                // cout << "Found if statement on line " << i << endl;
                line.erase(line.begin()); // The if
                line.erase(line.end() - 2); // The brace
                (*target).push_back(Block("if", constructAST(line, i), targetParent));
                // The parent pointer is now the current statement we're about to be nested inside of.
                // The target vector is the "nestedStatements" attribute of the statement we're about to be nested inside of.
                targetParent = &((*target).back());
            }
            else if(beginning.text == "else")
            {
                Block * oldIf = &((*target).back());
                Block * newElse = new Block("else", nullptr, oldIf);
                // cout << "Else statement on line " << i << " has if parent with condition " << printHelper(oldIf->condition, true) << endl;
                (*target).back().elseStatement = newElse;
                targetParent = newElse;
                // Redo the loop for an 'else if'
                if(line[1].text == "if")
                {
                    inputFromLexer[i].erase(inputFromLexer[i].begin());
                    i--;
                    continue;
                }
            }
            // While case is the same as if
            else if(beginning.text == "while")
            {
                line.erase(line.begin()); // The while
                line.erase(line.end() - 2); // The brace
                (*target).push_back(Block("while", constructAST(line, i), targetParent));
                // The parent pointer is now the current statement we're about to be nested inside of.
                // The target vector is the "nestedStatements" attribute of the statement we're about to be nested inside of.
                targetParent = &((*target).back());
            }

            // CASE BRACKET: A closed bracket.
            // Target should be redirected back up the control flow tree.
            else if(beginning.text == "}")
            {
                // If the most recent statement we encountered was an else statement, we have to go up 
                // 2 times, once to go into the else's parent "if", another time to go into the "if"'s parent.
                if(targetParent && (*targetParent).statementType == "else")
                {
                    targetParent = targetParent->parent;
                }
                targetParent = targetParent->parent;
                if(targetParent && (*targetParent).statementType == "else")
                {
                    targetParent = targetParent->parent;
                    targetParent = targetParent->parent;
                }
                // cout << "The closing brace on line " << i << "sets the parent to a statement with condition " << printHelper(targetParent->condition, true) << endl;
                continue;
            }
        }
        // cout << "There are " << blocks.size() << " top-level blocks" << endl;
    }
    
    // Delete any vectors that are nullptr
    // for (unsigned int i = 0; i < blocks.size(); i++)
    // {
    //     if (blocks[i].root == nullptr || blocks.size() == 1)
    //     {
    //         blocks.erase(blocks.begin() + i);
    //         i--;
    //     }
    // }
}

int getPrecedence(string token) // Helper function for constructAST
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

Node *Parser::constructAST(vector<Token> tokens, int line)
{
    // CHECK FOR ALL UNEXPECTED TOKEN ERRORS
    // The following function will print the error message on its own.
    // It returns true if there's an error detected.
    if (checkError(tokens, line) == true)
    {
        return nullptr;
    }
    // Remove the end token, which is no longer needed after checkError().
    tokens.pop_back();

    stack<Node *> nodeStack;   // will contain child and root nodes before linking to each other
    stack<string> stringStack; // will contain operators and parentheses, used for determining order of tree
    Node *root = nullptr;
    Node *child1 = nullptr;
    Node *child2 = nullptr;
    if (tokens.size() == 0 || (tokens.size() == 1 && tokens[0].isEnd())) // handles case where expression is empty line
    {
        return nullptr;
    }
    for (unsigned int i = 0; i < tokens.size(); i++)
    {
        if (tokens[i].text == "(") // signifies beginning of subtree
        {
            stringStack.push("(");
        }
        else if (tokens[i].isOperand()) // numbers and variables are treated the same at a base level
        {
            nodeStack.push(new Node{Node{tokens[i], vector<Node *>(), nullptr}});
            if (i == tokens.size() - 1 && !stringStack.empty()) // checks if it is the end of the expression and there is still linking to be done
            {
                while (!stringStack.empty() && stringStack.top() != "(")
                {
                    string currentString = stringStack.top();
                    root = new Node{
                        Node{Token{0, (int)i, currentString}, vector<Node *>(), nullptr}};
                    stringStack.pop();
                    child1 = nodeStack.top();
                    nodeStack.pop();
                    child2 = nodeStack.top();
                    nodeStack.pop();
                    root->branches.push_back(child2);
                    child2->parent = root;
                    root->branches.push_back(child1);
                    child1->parent = root;
                    nodeStack.push(root);
                }
            }
        }
        else if (tokens[i].text != ")") // operator case
        {
            while (!stringStack.empty() &&
                   stringStack.top() != "(" &&
                   ((tokens[i].text != "=" && getPrecedence(stringStack.top()) >= getPrecedence(tokens[i].text)) || (tokens[i].text == "=" && getPrecedence(stringStack.top()) > getPrecedence(tokens[i].text))))
            { // cases are treated differently based on if there is an = sign involved due to the right associativity of the = operator
                string currentString = stringStack.top();
                root = new Node{
                    Node{Token{0, (int)i + 1, currentString}, vector<Node *>(), nullptr}};
                stringStack.pop();
                child1 = nodeStack.top();
                nodeStack.pop();
                child2 = nodeStack.top();
                nodeStack.pop();
                root->branches.push_back(child2);
                child2->parent = root;
                root->branches.push_back(child1);
                child1->parent = root;
                nodeStack.push(root);
            }
            stringStack.push(tokens[i].text);
        }
        else if (tokens[i].text == ")") // signifies end of subtree
        {
            while (!stringStack.empty() && stringStack.top() != "(")
            {
                string currentString = stringStack.top();
                root = new Node{
                    Node{Token{0, (int)i + 1, currentString}, vector<Node *>(), nullptr}};
                stringStack.pop();
                child1 = nodeStack.top();
                nodeStack.pop();
                child2 = nodeStack.top();
                nodeStack.pop();
                root->branches.push_back(child2);
                child2->parent = root;
                root->branches.push_back(child1);
                child1->parent = root;
                nodeStack.push(root);
            }
            if (!stringStack.empty())
            { // checks if it is the end of the subtree and there is still linking to be done
                stringStack.pop();
                if (i == tokens.size() - 1)
                {
                    while (!stringStack.empty() && stringStack.top() != "(")
                    {
                        string currentString = stringStack.top();
                        root = new Node{
                            Node{Token{0, (int)i + 1, currentString}, vector<Node *>(), nullptr}};
                        stringStack.pop();
                        child1 = nodeStack.top();
                        nodeStack.pop();
                        child2 = nodeStack.top();
                        nodeStack.pop();
                        root->branches.push_back(child2);
                        child2->parent = root;
                        root->branches.push_back(child1);
                        child1->parent = root;
                        nodeStack.push(root);
                    }
                }
            }
        }
    }
    Node *finalRoot = nodeStack.top();
    return finalRoot;
}

void Parser::print() // Infix
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

        typedValue finalValue = evaluate(oneBlock.root);
        // cout << finalValue.type << "\n";
        string finalInfix = printHelper(oneBlock.root, true);
        outputPerExpression[i] << finalInfix << "\n";
        if(finalValue.isError())
        {
            outputPerExpression[i] << finalValue.outputError();
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

typedValue Parser::evaluate(Node *top)
{
    typedValue result = typedValue{DOUBLE, 0, ""};
    if (!top)
    {
        return result;
    }
    Token t = top->info;
    string text = top->info.text;
    // TYPE MISMATCH ERROR
    if(t.isOperator() && !(t.text == "="))
    {
        typedValue result1 = evaluate(top->branches[0]);
        typedValue result2 = evaluate(top->branches[1]);
        TypeTag type1 = evaluate(top->branches[0]).type;
        TypeTag type2 = evaluate(top->branches[1]).type;
        // cout << t.text << " " << type1 << " " << type2 << "\n";
        if (!(result1.isError() || result2.isError()) && (
            type1 != type2
        || (t.takesBoolsOnly() && (type1 == DOUBLE || type2 == DOUBLE))
        || (t.takesDoublesOnly() && (type1 == BOOLEAN || type2 == BOOLEAN))))
        {
            result.type = TYPEERROR;
            return(result);
        }
    }
    // DIVISION BY ZERO ERROR AND UNKNOWN IDENTIFIER ERROR
    if(t.isOperator())
    {
        typedValue result1 = evaluate(top->branches[0]);
        typedValue result2 = evaluate(top->branches[1]);
        // cout << result1.type << ", " << result2.type << "\n";
        if(t.text != "=") result.setType(result1.type);
        result.setType(result2.type);
        if(result1.type == IDERROR) 
        {
            // cout << "IDERROR FOUND " << "\n";
            result.unknownIDText = result1.unknownIDText;
        }
        if(result2.type == IDERROR)
        {
            // cout << "IDERROR FOUND " << "\n";
            result.unknownIDText = result2.unknownIDText;
        } 
    }
    
    
    if (text == "+")
    {
        result.data.doubleValue = evaluate(top->branches[0]).data.doubleValue;
        result.data.doubleValue += evaluate(top->branches[1]).data.doubleValue;
    }
    else if (text == "-")
    {
        result.data.doubleValue = evaluate(top->branches[0]).data.doubleValue;
        result.data.doubleValue -= evaluate(top->branches[1]).data.doubleValue;
    }
    else if (text == "%")
    {
        result.data.doubleValue = evaluate(top->branches[0]).data.doubleValue;
        for (unsigned int i = 1 ; i < top->branches.size() ; i++)
        {
            // Do modulus, but check for division by 0
            double d2 = evaluate(top->branches[i]).data.doubleValue;
            if (d2 == 0)
            {
                while (top->parent) top = top -> parent;
                result.type = DIVZEROERROR;
                result.data.doubleValue = std::numeric_limits<double>::quiet_NaN();
                return (result);
            }
            //result = result % d2;
            result.data.doubleValue = result.data.doubleValue - d2 * std::floor(result.data.doubleValue / d2);
        }
    }
    else if (text == "*")
    {
        result.data.doubleValue = evaluate(top->branches[0]).data.doubleValue;
        result.data.doubleValue *= evaluate(top->branches[1]).data.doubleValue;
    }
    else if (text == "/")
    {
        result = evaluate(top->branches[0]);
        for (unsigned int i = 1; i < top->branches.size(); i++)
        {
            // Divide, but check for division by 0 error
            double divisor = evaluate(top->branches[i]).data.doubleValue;
            if (divisor == 0)
            {
                // Find the root of the tree and print infix version (the tree should still print in infix form when a runtime error occurs!)
                while (top->parent)
                {
                    top = top->parent;
                }
                result.type = DIVZEROERROR;
                result.data.doubleValue = std::numeric_limits<double>::quiet_NaN();
                return (result);
            }
            result.data.doubleValue /= divisor;
        }
    }
    // The assignment operator is right-associative, so it evaluates the last (rightmost) child
    // of the operator in the AST to figure out what to assign these variables to.
    // doASSIGNMENT
    else if (text == "=")
    {
        // There are no assignee errors at this point (they were caught in checkError), assign all the operands to the value of the rightmost expression.
        result = evaluate(top->branches[1]);
        string key = top->branches[0]->info.text;
        provisional[key] = result;
    }
    else if (t.isNumber())
    {
        result = (top->info.getValue());
    }
    else if (t.isVariable())
    {
        // Test for undefined identifier error
        if (provisional.find(text) == provisional.end())
        {
            result.type = IDERROR;
            result.unknownIDText = text;
            result.data.doubleValue = std::numeric_limits<double>::quiet_NaN();
            return (result);
        }
        else
        {
            result = provisional[text];
        }
    }
    else if(t.isBoolean())
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = t.getValue().data.booleanValue;
    }
    // ALL LOGIC OPERATORS
    else if (text == "<")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue < evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == ">")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue > evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == ">=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue >= evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == "<=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.doubleValue <= evaluate(top->branches[1]).data.doubleValue);
    }
    else if (text == "==")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]) == evaluate(top->branches[1]));
    }
    else if (text == "!=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]) != evaluate(top->branches[1]));
    }
    else if (text == "|")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.booleanValue || evaluate(top->branches[1]).data.booleanValue);
    }
    else if (text == "&")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.booleanValue && evaluate(top->branches[1]).data.booleanValue);
    }
    else if (text == "^")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0]).data.booleanValue != evaluate(top->branches[1]).data.booleanValue);
    }
    return (result);
}

bool Parser::checkError(vector<Token> expression, int line) // runs before we try evaluating
{
    int lastIndex = expression.size() - 2;
    Token theEnd = expression.back();
    if (!theEnd.isEnd()) // make sure formatting of END is correct
    {
        cout << "ERROR: END TOKEN NOT PUSHED BACK TO EXPRESSION" << "\n";
        cout << expression.back().text << "\n";
        exit(4);
    }
    if (expression[lastIndex].isEnd()) // Covers weird case where we get 2 end tokens in 1 expression.
    {
        expression.pop_back();
        lastIndex--;
        Token theEnd = expression.back();
    }
    int parentheses = 0;
    for (int i = 0; i <= lastIndex; i++)
    {
        Token t = expression[i];
        // Statements are not supported if allowStatements is false. In this case they're errors.
        if(t.isStatement() && !(allowStatements))
        {
            parseError(t, line);
            return(true);
        }
        // Operators should have two operands between them.
        // The left operand can be a RIGHT parenthesis or a number or an identifier.
        // The right operand can be a LEFT parenthesis or a number or an identifier.
        // If the operand ocurrs at the beginning or ending of the vector, that's also bad.
        if (t.isOperator() || t.isOrderComparison())
        {
            if (i == 0 ||
                !(expression[i - 1].isOperand() || expression[i - 1].text == ")"))
            {
                // cout << "INVALID OPERATOR: CASE 1" << "\n";
                parseError(t, line);
                return (true);
            }
            else if (i == lastIndex)
            {
                // cout << "INVALID OPERATOR: CASE 2" << "\n";
                parseError(theEnd, line);
                return (true);
            }
            else if (!(expression[i + 1].isOperand() || expression[i + 1].text == "("))
            {
                // cout << "INVALID OPERATOR: CASE 3" << "\n";
                parseError(expression[i + 1], line);
                return (true);
            }
        }
        // The left of every equals sign should only be an identifier.
        if (t.text == "=")
        {
            if (!(expression[i - 1].isVariable()))
            {
                parseError(t, line);
                return (true);
            }
        }
        // Parentheses should be balanced.
        // There should never be an empty set of two closed parentheses.
        // After an open parentheses, we must see a number or an identifier or another open parenthesis.
        else if (t.text == "(")
        {
            parentheses++;
            if (i == lastIndex)
            {
                parseError(theEnd, line);
                return (true);
            }
            if (!(expression[i + 1].isOperand() || expression[i + 1].text == "(") || expression[i + 1].text == ")")
            {
                parseError(expression[i + 1], line);
                return (true);
            }
        }
        // Parentheses should be balanced.
        // Before a closed parentheses, we must see a number or an identifier or another closed parenthesis.
        else if (t.text == ")")
        {

            parentheses--;
            if (parentheses < 0) // This also covers the case where i == 0.
            {
                parseError(t, line);
                return (true);
            }
            if (!(expression[i - 1].isOperand() || expression[i - 1].text == ")"))
            {
                parseError(expression[i - 1], line);
                return (true);
            }
        }
        // Numbers and identifiers should have operators or parentheses to either side.
        // The parentheses on either side must only be open (when to the left) or closed (to the right).
        else if (t.isOperand())
        {
            // Check left
            if (i != 0 && !(expression[i - 1].text == "(" || expression[i - 1].isOperator()))
            {
                parseError(t, line);
                return (true);
            }
            // Check right
            else if (i != lastIndex && !(expression[i + 1].text == ")" || expression[i + 1].isOperator()))
            {
                parseError(expression[i + 1], line);
                return (true);
            }
        }
    }
    // At the very end of the expression, all the parentheses should be balanced.
    if (parentheses == 0)
    {
        return (false);
    }
    else
    {
        parseError(theEnd, line);
        return (true);
    }
}

void Parser::parseError(Token token, int line)
{
    if(exitImmediately)
    {   
        cout << "Unexpected token at line " << token.line << " column " << token.column << ": " << token.text << "\n"; 
        exit(2);
    }
    else
    {
        token.line = 1;
        outputPerExpression[line] << "Unexpected token at line " << token.line << " column " << token.column << ": " << token.text << "\n";
    }
    return;
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
        executeHelper(b);
    }
}

void Parser::executeHelper(Block b)
{
    if(b.statementType == "print")
    {
        cout << evaluate(b.root) << endl;
    }
    else if(b.statementType == "if")
    {
        typedValue conditionResult = evaluate(b.condition);
        if(conditionResult.type != BOOLEAN)
        {
            cout << "Runtime error: condition is not a bool." << endl;
            exit(3);
        }
        bool branchTaken = conditionResult.data.booleanValue;
        if(branchTaken)
        {
            for(Block nested : b.nestedStatements)
            {
                executeHelper(nested);
            }
        }
        else if (b.elseStatement)
        {
            executeHelper(*(b.elseStatement));
        }
    }
    else if(b.statementType == "else")
    {
        for(Block nested : b.nestedStatements)
        {
            executeHelper(nested);
        }
    }
    else if(b.statementType == "while")
    {
        typedValue conditionResult = evaluate(b.condition);
        if(conditionResult.type != BOOLEAN)
        {
            cout << "Runtime error: condition is not a bool." << endl;
            exit(3);
        }
        while(evaluate(b.condition).data.booleanValue)
        {
            for(Block nested : b.nestedStatements)
            {
                executeHelper(nested);
            }
        }
    }
    else
    {
        evaluate(b.root);
    }
    
}

void Parser::format()
{
    Block currentBlock;
    for(unsigned int i = 0; i < blocks.size(); i++)
    {
        currentBlock = blocks[i];
        formatHelper(currentBlock);
        cout << endl;
    }
}

void Parser::formatHelper(Block b, unsigned int indents)
{
    string type = b.statementType;
    // First, all the indents
    string whitespace = "";
    for(unsigned int i = 0; i < indents; i++)
    {
        whitespace += "    ";
    }
    cout << whitespace;
    // Then the content of the statements
    if(type == "expression")
    {
        cout << printHelper(b.root, true);
    }
    else if(type == "print")
    {
        cout << "print " << printHelper(b.root, true);
    }
    else if(type == "if" || type == "while" || type == "else")
    {
        if(b.condition)
        {   
            cout << b.statementType << " " << printHelper(b.condition, true) << " {" << endl;
        }
        else
        {
            cout << b.statementType << " {" << endl;
        }
        
        for(Block nestedBlock : b.nestedStatements)
        {
            formatHelper(nestedBlock, indents + 1);
            cout << endl;
        }
        cout << whitespace << "}";
        if(b.elseStatement)
        {
            cout << endl;
            formatHelper(*(b.elseStatement), indents);
        }
    }
}
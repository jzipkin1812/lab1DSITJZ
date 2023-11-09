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
            // Find the correct block vector to push things into. If parent is nullptr, that means we're not nested in anything and it should just be blocks.
            if(!targetParent)
            {
                target = &(blocks);
            }
            else
            {
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
                (*target).push_back(Block(constructAST(line, i, true)));
            }
            // CASE 2: A print statement.
            // Pop the print token. Construct an AST. Push a new block to target with this AST and type "print".
            // We don't need to track the parents of print statements. They don't nest.
            else if(beginning.text == "print" || beginning.text == "return")
            {
                line.erase(line.begin());
                (*target).push_back(Block(beginning.text, constructAST(line, i, true), targetParent));
            }
            // CASE 3: An if statement.
            // Pop the if token. Pop the brace. Construct an AST for the condition. 
            // Change "target" to this if statement's nestedStatements vector so that statements on future lines are added to it.
            else if(beginning.text == "if")
            {
                line.erase(line.begin()); // The if
                line.erase(line.end() - 2); // The brace
                (*target).push_back(Block("if", constructAST(line, i), targetParent));
                // The parent pointer is now the current statement we're about to be nested inside of.
                // The target vector is the "nestedStatements" attribute of the statement we're about to be nested inside of.
                targetParent = &((*target).back());
            }
            else if(beginning.text == "else")
            {
                Block * newElse = new Block("else", nullptr, targetParent);
                targetParent->elseStatement = newElse;
                targetParent = newElse;
                // Detect whether this is the last else in the chain
                vector<Token> oneBracket = {Token(0, 0, "}")};
                unsigned int nextClosedIndex = nextClose(inputFromLexer, i);
                bool lastElseChain = (nextClosedIndex == inputFromLexer.size() - 1) || (inputFromLexer[nextClosedIndex + 1][0].text != "else");
                // Add another bracket at the proper location if it's the last one
                if(lastElseChain)
                {
                    inputFromLexer.insert(inputFromLexer.begin() + nextClosedIndex, oneBracket);
                } 
                // Redo the loop for an 'else if'
                if(line[1].text == "if")
                {
                    if(lastElseChain)
                    {
                        inputFromLexer.insert(inputFromLexer.begin() + nextClosedIndex, oneBracket);
                    } 
                    // Redo code
                    inputFromLexer[i].erase(inputFromLexer[i].begin());
                    i--;
                    continue;
                }
            }
            // While case
            else if(beginning.text == "while")
            {
                line.erase(line.begin()); // The while
                line.erase(line.end() - 2); // The brace
                (*target).push_back(Block(beginning.text, constructAST(line, i), targetParent));
                // The parent pointer is now the current statement we're about to be nested inside of.
                // The target vector is the "nestedStatements" attribute of the statement we're about to be nested inside of.
                targetParent = &((*target).back());
            }

            // Def statement
            else if(beginning.text == "def")
            {
                line.erase(line.begin()); // The while
                line.erase(line.end() - 2); // The brace
                Block defBlock;
                defBlock.statementType = "def"; // Def
                defBlock.functionName = (*line.begin()).text; // Name of new function
                line.erase(line.begin()); // Get rid of the function name
                for(unsigned int j = 0; j < line.size(); j++)
                {
                    Token possibleArgument = line[j];
                    if(possibleArgument.isVariable())
                    {
                        defBlock.argumentNames.push_back(possibleArgument.text);
                    }
                }
                defBlock.parent = targetParent;

                (*target).push_back(defBlock);
                targetParent = &((*target).back());
            }

            // CASE BRACKET: A closed bracket.
            // Target should be redirected back up the control flow tree.
            else if(beginning.text == "}")
            {
                if(targetParent && (*targetParent).statementType == "if" && inputFromLexer[i + 1][0].text == "else")
                {
                    continue;
                }
                targetParent = targetParent->parent;                
                continue;
            }
        }
    }
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

Node *Parser::constructAST(vector<Token> tokens, int line, bool requireSemicolons)
{
    if (line == 90000) return nullptr;
    
    // CHECK FOR ALL UNEXPECTED TOKEN ERRORS
    // The following function will print the error message on its own.
    // It returns true if there's an error detected.
    //if (checkError(tokens, line, requireSemicolons) == true)
    //{
       // return nullptr;
    //}
    // Remove the end token, which is no longer needed after checkError().
    tokens.pop_back();
    // Remove the semicolon token, which is no longer needed after checkError().
    if(requireSemicolons)
    {
        tokens.pop_back();
    }

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
            if (i+1<tokens.size() && tokens[i+1].text == "("){

                unsigned int originalIndex = i;
                int parenCount = 0;
                vector<Token> argument;
                root = new Node{tokens[i], vector<Node *>(), nullptr};
                i = i + 2;
                // cout << "Reached a function call..." << endl;
                while (true) 
                {
                    if (tokens[i].text == "(") parenCount++;
                    if (tokens[i].text == ")") parenCount--;
                    if (parenCount < 0)
                    {
                        // cout << "Constructing argument with size " << argument.size() << endl;
                        argument.push_back(Token(0, 0, ")"));
                        argument.push_back(Token(0, 0, ";"));
                        argument.push_back(Token(0, 0, "END"));
                        Node * temp = constructAST(argument);
                        // cout << "Pushing temp with root text " << temp->info.text << endl;
                        temp->parent = root;
                        root->branches.push_back(temp);
                        argument.clear();
                        break;
                    }
                    else if (tokens[i].text == "," && parenCount == 0)
                    {
                        // cout << "Constructing argument with size " << argument.size() << endl;
                        argument.push_back(Token(0, 0, ")"));
                        argument.push_back(Token(0, 0, ";"));
                        argument.push_back(Token(0, 0, "END"));
                        Node * temp = constructAST(argument);
                        // cout << "Pushing temp with root text " << temp->info.text << endl;
                        temp->parent = root;
                        root->branches.push_back(temp);
                        argument.clear();
                        argument.push_back(Token(0, 0, "("));
                    }
                    else { argument.push_back(tokens[i]); }
                    i++;
                }
                tokens.erase(tokens.begin() + originalIndex + 1, tokens.begin() + i + 1);
                nodeStack.push(root);
                i = originalIndex;
                // cout << "Tokens[i - 1] is now \"" << tokens[i - 1].text << "\"" << endl;
            }
            else{ nodeStack.push(new Node{tokens[i], vector<Node *>(), nullptr}); }

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
    /* if ((expectedValue == "Boolean") && (evaluate(finalRoot).type != BOOLEAN))
    {
        cout << "Runtime error: condition is not a bool." << endl;
    } */
    return finalRoot;
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

typedValue Parser::evaluate(Node *top, map<string, typedValue>& scopeMap)
{
    typedValue result = typedValue{DOUBLE, {0}, ""};
    if (!top)
    {
        return result;
    }
    Token t = top->info;
    string text = top->info.text;
    // TYPE MISMATCH ERROR
    if(t.isOperator() && !(t.text == "="))
    {
        typedValue result1 = evaluate(top->branches[0], scopeMap);
        typedValue result2 = evaluate(top->branches[1], scopeMap);
        TypeTag type1 = evaluate(top->branches[0], scopeMap).type;
        TypeTag type2 = evaluate(top->branches[1], scopeMap).type;
        if (!(result1.isError() || result2.isError()) && (
            ((type1 != type2) && (t.text != "==" && t.text != "!="))
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
        typedValue result1 = evaluate(top->branches[0], scopeMap);
        typedValue result2 = evaluate(top->branches[1], scopeMap);
        if(t.text != "=") result.setType(result1.type);
        result.setType(result2.type);
        if(result1.type == IDERROR) 
        {
            result.unknownIDText = result1.unknownIDText;
        }
        if(result2.type == IDERROR)
        {
            result.unknownIDText = result2.unknownIDText;
        } 
    }
    
    if (text == "+")
    {
        result.data.doubleValue = evaluate(top->branches[0], scopeMap).data.doubleValue;
        result.data.doubleValue += evaluate(top->branches[1], scopeMap).data.doubleValue;
    }
    else if (text == "-")
    {
        result.data.doubleValue = evaluate(top->branches[0], scopeMap).data.doubleValue;
        result.data.doubleValue -= evaluate(top->branches[1], scopeMap).data.doubleValue;
    }
    else if (text == "%")
    {
        result.data.doubleValue = evaluate(top->branches[0], scopeMap).data.doubleValue;
        for (unsigned int i = 1 ; i < top->branches.size() ; i++)
        {
            // Do modulus, but check for division by 0
            double d2 = evaluate(top->branches[i], scopeMap).data.doubleValue;
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
        result.data.doubleValue = evaluate(top->branches[0], scopeMap).data.doubleValue;
        result.data.doubleValue *= evaluate(top->branches[1], scopeMap).data.doubleValue;
    }
    else if (text == "/")
    {
        result = evaluate(top->branches[0], scopeMap);
        for (unsigned int i = 1; i < top->branches.size(); i++)
        {
            // Divide, but check for division by 0 error
            double divisor = evaluate(top->branches[i], scopeMap).data.doubleValue;
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
        // Check for the invalid assignee runtime error
        Token assignee = top->branches[0]->info;
        if(assignee.isVariable())
        {
            // There are no assignee errors at this point. Assign the variables.
            result = evaluate(top->branches[1], scopeMap);
            string key = top->branches[0]->info.text;
            scopeMap[key] = result;
        }
        else
        {
            // Invalid assignee error.
            result.type = ASSIGNEEERROR;
        }
        
    }
    else if (t.isNumber())
    {
        result = (top->info.getValue());
    }
    else if (t.isVariable())
    {
        // Test for undefined identifier error
        if (scopeMap.find(text) == scopeMap.end())
        {
            result.type = IDERROR;
            result.unknownIDText = text;
            return (result);
        }
        else
        {
            result = scopeMap[text];
            // But...if result is a function, we need to call the function.
            if(result.type == FUNCTION)
            {
                Func * converted = reinterpret_cast<Func*>(result.data.functionValue);
                vector<typedValue> evaluatedArguments;
                for(Node * child : top->branches)
                {
                    evaluatedArguments.push_back(evaluate(child, scopeMap));
                }
                result = callFunction(*converted, evaluatedArguments);
            }
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
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.doubleValue < evaluate(top->branches[1], scopeMap).data.doubleValue);
    }
    else if (text == ">")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.doubleValue > evaluate(top->branches[1], scopeMap).data.doubleValue);
    }
    else if (text == ">=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.doubleValue >= evaluate(top->branches[1], scopeMap).data.doubleValue);
    }
    else if (text == "<=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.doubleValue <= evaluate(top->branches[1], scopeMap).data.doubleValue);
    }
    else if (text == "==")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap) == evaluate(top->branches[1], scopeMap));
    }
    else if (text == "!=")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap) != evaluate(top->branches[1], scopeMap));
    }
    else if (text == "|")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.booleanValue || evaluate(top->branches[1], scopeMap).data.booleanValue);
    }
    else if (text == "&")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.booleanValue && evaluate(top->branches[1], scopeMap).data.booleanValue);
    }
    else if (text == "^")
    {
        result.setType(BOOLEAN);
        result.data.booleanValue = (evaluate(top->branches[0], scopeMap).data.booleanValue != evaluate(top->branches[1], scopeMap).data.booleanValue);
    }
    return (result);
}

bool Parser::checkError(vector<Token> expression, int line, bool requireSemicolons) // runs before we try evaluating
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
    // Check for the semicolon right away. If it's there, we can decrement last index.
    if(requireSemicolons)
    {
        if(expression[lastIndex].text != ";" && expression.size())
        {
            parseError(theEnd, line);
            return(true);
        }
        else
        {
            lastIndex--;
        }
    }

    int parentheses = 0;
    for (int i = 0; i <= lastIndex; i++)
    {
        Token t = expression[i];
        // Statements are not supported if allowStatements is false. In this case they're errors.
        if(t.isStatement() && !(allowStatements))
        {
            cout << "CASE 1" << endl;
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
        // Assignee errors are no longer caught until runtime.
        // Parentheses should be balanced.
        // There should never be an empty set of two closed parentheses.
        // After an open parentheses, we must see a number or an identifier or another open parenthesis.
        else if (t.text == "(")
        {
            parentheses++;
            if (i == lastIndex)
            {
                // cout << "CASE 2" << endl;
                parseError(theEnd, line);
                return (true);
                
            }
            if (!(expression[i + 1].isOperand() || expression[i + 1].text == "(") || expression[i + 1].text == ")")
            {
                // cout << "CASE 2" << endl;
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
                // cout << "CASE 3" << endl;
                parseError(t, line);
                return (true);
            }
            // Check right
            else if (i != lastIndex && !(expression[i + 1].text == ")" || expression[i + 1].isOperator()))
            {
                // cout << "CASE 4" << endl;
                parseError(expression[i + 1], line);
                return (true);
            }
        }

        // Semicolons should not show up at all. If they were at the end of an expression,
        // then they should already have been ignored by a lastIndex decrement.
        else if(t.isSemicolon())
        {
            parseError(t, line);
            return(true);
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
        executeHelper(b, provisional);
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

typedValue Parser::executeHelper(Block b, map<string, typedValue>& scope)
{
    typedValue noneReturn;
    typedValue doReturn;
    noneReturn.type = NONE;
    if(b.statementType == "print")
    {
        typedValue printResult = evaluate(b.root, scope);
        if(printResult.isError()) printResult.outputError(true);
        cout << printResult << endl;
    }
    else if(b.statementType == "if")
    {
        typedValue conditionResult = evaluate(b.condition, scope);
        if(conditionResult.type != BOOLEAN) conditionResult.setType(NOCONDITIONERROR);
        if(conditionResult.isError()) conditionResult.outputError(true);
        bool branchTaken = conditionResult.data.booleanValue;
        if(branchTaken)
        {
            for(Block nested : b.nestedStatements)
            {
                doReturn = executeHelper(nested, scope);
                if(doReturn.type != NONE) return doReturn;
            }
        }
        else if (b.elseStatement)
        {
            doReturn = executeHelper(*(b.elseStatement), scope);
            if(doReturn.type != NONE) return doReturn;
        }
    }
    else if(b.statementType == "else")
    {
        for(Block nested : b.nestedStatements)
        {
            doReturn = executeHelper(nested, scope);
            if(doReturn.type != NONE) return doReturn;
        }
    }
    else if(b.statementType == "while")
    {
        typedValue conditionResult = evaluate(b.condition, scope);
        // If the condition is not boolean or another runtime error occurs in a condition, exit
        if(conditionResult.type != BOOLEAN) conditionResult.setType(NOCONDITIONERROR);
        if(conditionResult.isError()) conditionResult.outputError(true);
        while(conditionResult.data.booleanValue)
        {
            for(Block nested : b.nestedStatements)
            {
                doReturn = executeHelper(nested, scope);
                if(doReturn.type != NONE) return doReturn;
            }
            // If the condition is not boolean, exit
            conditionResult = evaluate(b.condition, scope);
            if(conditionResult.type != BOOLEAN)
            {
                cout << "Runtime error: condition is not a bool." << endl;
                exit(3);
            }
        }
    }
    else if(b.statementType == "def")
    {
        // Capture variables
        b.capturedVariables = scope;
        // cout << "New captured variables has address " << &b.capturedVariables << " and is capturing from address " << &scope << endl;
        Func * newFunction = new Func(b, b.capturedVariables);
        newFunction->capturedVariables = b.capturedVariables;
        typedValue functionStorage; // Stores the new function in a typedValue.
        functionStorage.type = FUNCTION; // This typedvalue is of type FUNCTION.
        functionStorage.data.functionValue = newFunction; 
        scope[b.functionName] = functionStorage; // Remember the function for later.
    }
    else if(b.statementType == "return")
    {
        typedValue returnResult = evaluate(b.root, scope);
        if(returnResult.isError()) returnResult.outputError(true);
        return(returnResult);
    }
    else // Case for expression
    {
        typedValue expressionResult = evaluate(b.root, scope);
        // cout << expressionResult.type << endl;
        if(expressionResult.isError()) expressionResult.outputError(true);
    }

    return(noneReturn);
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
        cout << printHelper(b.root, true) << ";";
    }
    else if(type == "print" || type == "return")
    {
        cout << type << " " << printHelper(b.root, true) << ";";
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
    if(type == "def")
    {
        cout << "def " << b.functionName << "(";
        for(unsigned int arg = 0; arg < b.argumentNames.size(); arg++)
        {
            cout << b.argumentNames[arg];
            if(arg < b.argumentNames.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << ") {\n";
        for(Block nestedBlock : b.nestedStatements)
        {
            formatHelper(nestedBlock, indents + 1);
            cout << endl;
        }
        cout << whitespace << "}";
    }
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

typedValue Parser::callFunction(Func givenFunction, vector<typedValue> arguments)
{
    // Stop using capturedVariables (which is only useful in a def statement) and start using Variables (which belongs to this specific function call)
    givenFunction.variables = givenFunction.capturedVariables;

    // Capture parameters passed in by value 
    for(unsigned int i = 0; i < arguments.size(); i++)
    {
        givenFunction.variables[givenFunction.argumentNames[i]] = arguments[i];
    }

    typedValue nullValue;
    nullValue.type = NONE;

    for(Block b : givenFunction.nestedStatements)
    {
        typedValue returned = executeHelper(b, givenFunction.variables);
        if(returned.type != NONE)
        {
            return(returned);
        }
    }

    return(nullValue);
}
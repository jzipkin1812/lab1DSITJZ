#include <iostream>
#include <string>
#include <vector>
#include "token.h"
#include "lex.h"
#include "parser_infix.h"
using namespace std;

ParserInfix::ParserInfix(vector<vector<Token>> inVectors) 
{
    vectors = inVectors;
    for (vector<Token> tokens : inVectors)
    {
        // The expression is just a number
        if(tokens.size() == 2 && tokens[0].isNumber())
        {
            roots.push_back(new Node{ParserInfix::Node{tokens[0], vector<Node*>(), nullptr}});
            return;
        }

        // ERROR - The expression is empty 
        else if(tokens.size() < 2)
        {
            if(tokens[0].line != 1) 
                tokens[0].line++;
            parseError(tokens[0]);
        }

        else // The expression is ok
        {
            Node* root = new Node{ParserInfix::Node{tokens[0], vector<Node*>(), nullptr}};
            roots.push_back(root);



        }


    }
}
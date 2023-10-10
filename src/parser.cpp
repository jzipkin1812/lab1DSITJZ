#include <iostream>
#include <string>
#include "lib/parser.h"
using namespace std;

Parser::Parser(vector<Token> tokens);
double Parser::evaluate();
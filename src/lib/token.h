#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>
using namespace std;

struct Token
{
    int line;
    int column; // first character = token number
    string text;
};

#endif
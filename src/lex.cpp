#include <iostream>
#include <vector>
#include <iomanip>
#include <ctype.h>
#include "lib/lex.h"
#include "lib/parse.h"
#include "lib/token.h"
using namespace std;

int main()
{
    Lexer myLexer = Lexer(false, true);
    myLexer.print();
    return (0);
}
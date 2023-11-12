#include <iostream>
#include <string>
#include <vector>
#include "lib/parse.h"
#include "lib/token.h"
#include "lib/lex.h"

using namespace std;

int main(int argc, char *argv[])
{
    Lexer myLexer = Lexer(true, true, "input.txt");
    Parser myParser = Parser(myLexer.getTokens(), true);
    if(argc == 1 || argv[1][0] == '0')
    {
        myParser.execute(); // Calls evaluate. Only prints when prompted.
    }
    else
    {
        myParser.format();
    }
    
    return 0;
}
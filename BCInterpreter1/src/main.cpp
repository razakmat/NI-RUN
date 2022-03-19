#include <iostream>

#include "Parser.hpp"
#include "FMLVM.hpp"
#include "Interpreter.hpp"

using namespace std;


int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        cout << "Wrong number of arguments" << endl;
        return 0;
    }

    Parser * parse = new Parser();
    parse->LoadFile(argv[1]);

    FMLVM * vm = new FMLVM();
    parse->ParseCode(vm);
    delete parse;

    Interpreter * eval = new Interpreter(vm);
    try{
        eval->Run();
    }catch(const char * e)
    {
        cout << e << endl;
    }

    delete eval;
    return 0;
}

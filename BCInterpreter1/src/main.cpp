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
    FMLVM * vm = new FMLVM();

    try{
        parse->LoadFile(argv[1]);
        parse->ParseCode(vm);
    }catch(const char * e)
    {
        cout << e << endl;
        delete parse;
        delete vm;
        return 0;
    }
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

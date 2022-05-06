#include <iostream>

#include "Parser.hpp"
#include "FMLVM.hpp"
#include "Interpreter.hpp"
#include <string.h>

using namespace std;


int main(int argc, char ** argv)
{
    if (argc != 6)
    {
        cout << "Wrong number of arguments" << endl;
        return 0;
    }
    int heap_size;
    bool log = true;
    int log_arg = 0;

    if (strcmp(argv[2],"--heap-size") == 0)
        heap_size = atoi(argv[3]);
    else if (strcmp(argv[4],"--heap-size") == 0)
        heap_size = atoi(argv[5]);
    else
        heap_size = 0;
    if (strcmp(argv[2],"--heap-log") == 0)
        log_arg = 3;
    else if (strcmp(argv[4],"--heap-log") == 0)
        log_arg = 5;
    else
        log = false;


    Parser * parse = new Parser();
    FMLVM * vm = new FMLVM(heap_size);

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

    if (log)
        vm->m_heap.WriteHeapLog(argv[log_arg]);

    delete eval;
    return 0;
}

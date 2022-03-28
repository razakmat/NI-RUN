#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include "FMLVM.hpp"
#include "OpStack.hpp"
#include <vector>

using namespace std;

class Interpreter
{
    public:
        Interpreter(FMLVM * vm);
        ~Interpreter();
        void Run();
        void operator()(ILiteral & literal);
        void operator()(IDrop & drop);
        void operator()(IPrint & print);
        void operator()(IJump & jump);
        void operator()(IBranch & branch);
        void operator()(ILabel & label);
        void operator()(IGet_Local & getLocal);
        void operator()(ISet_Local & setLocal);
        void operator()(IGet_Global & getGlobal);
        void operator()(ISet_Global & setGlobal);
        void operator()(ICall_Function & call);
        void operator()(IReturn & ret);
        void operator()(IArray & arr);
        void operator()(IObject & obj);
        void operator()(IGet_Field & getField);
        void operator()(ISet_Field & setField);
        void operator()(ICall_Method & callMethod);

        template <typename A>
        void operator()(A & pok);
    private:
        void print_out(int16_t args,const string & str);
        FMLVM * m_vm;
        uint32_t m_PC;
};


#endif // __INTERPRETER_H__
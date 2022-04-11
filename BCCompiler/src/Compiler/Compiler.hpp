#ifndef COMPILER_H
#define COMPILER_H

#include "../Structures/ProgramObject.hpp"
#include "../Structures/Instruction.hpp"
#include "../Visitor/Visitor.h"
#include "../Structures/AST.h"

#include <variant>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <stack>


using namespace std;


class Compiler : public Visitor
{
    public:
        Compiler();
        unsigned char * Final(uint64_t & size);
        ASTValue * visit ( ASTInteger * integer);
        ASTValue * visit ( ASTBoolean * boolean);
        ASTValue * visit ( ASTNull * null);
        ASTValue * visit ( ASTVariable * variable);
        ASTValue * visit ( ASTAccessVariable * accessVar);
        ASTValue * visit ( ASTAssignVariable * assignVar);
        ASTValue * visit ( ASTFunction * fun);
        ASTValue * visit ( ASTCallFunction * callFun);
        ASTValue * visit ( ASTPrint * print);
        ASTValue * visit ( ASTBlock * block);
        ASTValue * visit ( ASTTop * top);
        ASTValue * visit ( ASTLoop * loop);
        ASTValue * visit ( ASTConditional * cond);
        ASTValue * visit ( ASTObject * obj);
        ASTValue * visit ( ASTArray * arr);
        ASTValue * visit ( ASTAssignArray * assignArr);
        ASTValue * visit ( ASTAccessArray * accessArr);
        ASTValue * visit ( ASTAssignField * assignFie);
        ASTValue * visit ( ASTAccessField * accessFie);
        ASTValue * visit ( ASTCallMethod * call);
    protected:
        uint16_t InsertConst(const string & str, constant con);
        uint16_t CreateStringToConst(const string & str);
        bool FindName(const string & str, uint16_t & index,bool global = false);
        void DefineGlobalVar(ASTVariable * var);
        void DefineGlobalFunction(ASTFunction * fun);
        void reallocate(unsigned char *& data,uint64_t size, uint64_t max);

        template <typename T>
        void SetAndPush(T i, uint16_t index);
        vector<uint16_t> m_globals;
        shared_ptr<vector<ins>> m_code;
        vector<shared_ptr<vector<ins>>> m_codes;
        vector<constant> m_constant_pool;
        struct frame{
            unordered_map<string,uint16_t> m_varMap;
            shared_ptr<frame> m_prev;
            bool m_endOfScope;
            int m_sizePrev;
        };
        shared_ptr<frame> m_frame;
        shared_ptr<frame> m_globalFrame;
        vector<int> m_vector_locals;
        stack<bool> m_to_drop;
        int m_counter_locals;
        uint16_t m_entry;
        const string m_nameOfmain = "λ:";
        int m_counterLabel = 0;
        bool m_method = false;
};

#endif

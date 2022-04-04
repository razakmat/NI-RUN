#ifndef VISITOR_H
#define VISITOR_H

#include "../Structures/AST.h"

using namespace std;



class Visitor
{
    public:
        virtual ASTValue * visit ( ASTInteger * integer) = 0;
        virtual ASTValue * visit ( ASTBoolean * boolean) = 0;
        virtual ASTValue * visit ( ASTNull * null) = 0;
        virtual ASTValue * visit ( ASTVariable * variable) = 0;
        virtual ASTValue * visit ( ASTAccessVariable * accessVar) = 0;
        virtual ASTValue * visit ( ASTAssignVariable * assignVar) = 0;
        virtual ASTValue * visit ( ASTFunction * fun) = 0;
        virtual ASTValue * visit ( ASTCallFunction * callFun) = 0;
        virtual ASTValue * visit ( ASTPrint * print) = 0;
        virtual ASTValue * visit ( ASTBlock * block) = 0;
        virtual ASTValue * visit ( ASTTop * top) = 0;
        virtual ASTValue * visit ( ASTLoop * loop) = 0;
        virtual ASTValue * visit ( ASTConditional * cond) = 0;
        virtual ASTValue * visit ( ASTObject * obj) = 0;
        virtual ASTValue * visit ( ASTArray * arr) = 0;
        virtual ASTValue * visit ( ASTAssignArray * assignArr) = 0;
        virtual ASTValue * visit ( ASTAccessArray * accessArr) = 0;
        virtual ASTValue * visit ( ASTAssignField * assignFie) = 0;
        virtual ASTValue * visit ( ASTAccessField * accessFie) = 0;
        virtual ASTValue * visit ( ASTCallMethod * call) = 0;


};







#endif
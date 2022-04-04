#ifndef AST_H
#define AST_H

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

class Visitor;
class ASTValue;

class AST
{
    public:
        enum class Type{
            Null = 0,
            Integer = 1,
            BoolTrue = 2,
            BoolFalse = 3,
            Array = 4,
            Object = 5
        };
        virtual  ASTValue * Accept(Visitor & v) = 0;
        virtual ~AST() = default;
};


class ASTValue : public AST
{
    public:
        virtual ASTValue * Accept(Visitor & v) = 0;
        virtual ~ASTValue() = default;
        virtual AST::Type Type() = 0;
        virtual string Print() = 0;
};

class ASTInteger : public ASTValue
{
    public:
        ASTInteger(const int32_t value);
        ASTValue * Accept(Visitor & v);
        AST::Type Type();
        string Print();
        const int32_t m_value;
};

class ASTBoolean : public ASTValue
{
    public:
        ASTBoolean(const bool value);
        ASTValue * Accept(Visitor & v);
        AST::Type Type();
        string Print();
        const bool m_value;

};

class ASTNull : public ASTValue
{
    public:
        ASTValue * Accept(Visitor & v);
        AST::Type Type();
        string Print();
};

class ASTVariable : public AST
{
    public:
        ASTVariable(const string & name, AST * value);
        ~ASTVariable();
        ASTValue * Accept(Visitor & v);
        const string m_name;
        AST * m_value;
};

class ASTAccessVariable : public AST
{
    public:
        ASTAccessVariable(const string & name);
        ASTValue * Accept(Visitor & v);
        const string m_name;

};

class ASTAssignVariable : public AST
{
    public:
        ASTAssignVariable(const string & name, AST * value);
        ~ASTAssignVariable();
        ASTValue * Accept(Visitor & v); 
        const string m_name;
        AST * m_value;
};

class ASTArray : public ASTValue
{
    public:
        ASTArray(AST * size, AST * value);
        ASTArray();
        ~ASTArray();
        ASTValue * Accept(Visitor & v);
        AST::Type Type();
        string Print();
        AST * m_size;
        AST * m_value;
        int m_heap_ref;
        vector<ASTValue*> m_elements;
};

class ASTAccessArray : public AST
{
    public:
        ASTAccessArray(AST * array, AST * index);
        ~ASTAccessArray();
        ASTValue * Accept(Visitor & v);
        AST * m_array;
        AST * m_index;
};

class ASTAssignArray : public AST
{
    public:
        ASTAssignArray(AST * array, AST * index, AST * value);
        ~ASTAssignArray();
        ASTValue * Accept(Visitor & v);
        AST * m_array;
        AST * m_index;
        AST * m_value;
};

class ASTFunction : public AST
{
    public:
        ASTFunction(const string & name, vector<string> & param,
                    AST * body,const string & json);
        ~ASTFunction();
        ASTValue * Accept(Visitor & v);
        const string m_name;
        vector<string> m_param;
        AST * m_body;
        const string m_json; 
};

class ASTCallFunction : public AST
{
    public:
        ASTCallFunction(const string & name, vector<AST*> & arg);
        ~ASTCallFunction();
        ASTValue * Accept(Visitor & v);
        const string m_name;
        vector<AST*> m_arg;
};

class ASTPrint : public AST
{
    public:
        ASTPrint(const string & format, vector<AST*> & arg);
        ~ASTPrint();
        ASTValue * Accept(Visitor & v);
        const string m_format;
        vector<AST*> m_arg;
};

class ASTBlock : public AST
{
    public:
        ASTBlock(vector<AST*> & stmt);
        ~ASTBlock();
        ASTValue * Accept(Visitor & v);
        vector<AST*> m_stmt;
};

class ASTTop : public AST
{
    public:
        ASTTop(vector<AST*> & stmt);
        ~ASTTop();
        ASTValue * Accept(Visitor & v);
        vector<AST*> m_stmt;
};

class ASTLoop : public AST
{
    public:
        ASTLoop(AST * cond, AST * body);
        ~ASTLoop();
        ASTValue * Accept(Visitor & v);
        AST * m_cond;
        AST * m_body;
};

class ASTConditional : public AST
{
    public:
        ASTConditional(AST * cond, AST * _if, AST * _else);
        ~ASTConditional();
        ASTValue * Accept(Visitor & v);
        AST * m_cond;
        AST * m_if;
        AST * m_else;
};

class ASTObject : public ASTValue
{
    public:
        ASTObject(AST * extends, vector<AST*> & members);
        ASTObject();
        ~ASTObject();
        ASTValue * Accept(Visitor & v);
        AST::Type Type();
        string Print();
        AST * m_extends;
        vector<AST*> m_members;
        ASTValue * m_prev;
        void * m_stack;
};

class ASTAssignField : public AST
{
    public:
        ASTAssignField(AST * object, const string & field, AST * value);
        ~ASTAssignField();
        ASTValue * Accept(Visitor & v);
        AST * m_object;
        const string m_field;
        AST * m_value;
};

class ASTAccessField : public AST
{
    public:
        ASTAccessField(AST * object, const string & field);
        ~ASTAccessField();
        ASTValue * Accept(Visitor & v);
        AST * m_object;
        const string m_field;
};

class ASTCallMethod : public AST
{
    public:
        ASTCallMethod(AST * object, const string & name, vector<AST*> & arg);
        ~ASTCallMethod();
        ASTValue * Accept(Visitor & v);
        AST * m_object;
        const string m_name;
        vector<AST*> m_arg;
};


#endif
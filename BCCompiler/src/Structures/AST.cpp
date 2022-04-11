#include "../Structures/AST.h"
#include "../Visitor/Visitor.h"

using namespace std;


ASTInteger::ASTInteger(const int32_t value)
        : m_value(value) 
{ 
}

ASTValue * ASTInteger::Accept(Visitor & v)
{
    return v.visit(this);
}

AST::Type ASTInteger::Type()
{
    return AST::Type::Integer;
}

string ASTInteger::Print()
{
    return to_string(m_value);
}



ASTBoolean::ASTBoolean(const bool value)
:m_value(value) 
{    
}

ASTValue * ASTBoolean::Accept(Visitor & v)
{
    return v.visit(this);
}

AST::Type ASTBoolean::Type()
{
    if (m_value)
        return AST::Type::BoolTrue;
    return AST::Type::BoolFalse;
}

string ASTBoolean::Print()
{
    if (m_value)
        return "True";
    return "False";
}



ASTValue * ASTNull::Accept(Visitor & v)
{
    return v.visit(this);
}

AST::Type ASTNull::Type()
{
    return AST::Type::Null;
}

string ASTNull::Print()
{
    return "Null";
}



ASTVariable::ASTVariable(const string & name, AST * value)
: m_name(name), m_value(value) 
{
}

ASTValue * ASTVariable::Accept(Visitor & v)
{
    return v.visit(this);
}

ASTVariable::~ASTVariable()
{
    delete m_value;
}



ASTAccessVariable::ASTAccessVariable(const string & name)
: m_name(name) 
{
}

ASTValue * ASTAccessVariable::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTAssignVariable::ASTAssignVariable(const string & name, AST * value)
: m_name(name) , m_value(value) 
{    
}

ASTValue * ASTAssignVariable::Accept(Visitor & v)
{
    return v.visit(this);
}

ASTAssignVariable::~ASTAssignVariable()
{
    delete m_value;
}



ASTArray::ASTArray(AST * size, AST * value)
: m_size(size) , m_value(value) 
{
}

ASTArray::ASTArray()
{
}


ASTArray::~ASTArray()
{
    delete m_size;
    delete m_value;
}



ASTValue * ASTArray::Accept(Visitor & v)
{
    return v.visit(this);
}

AST::Type ASTArray::Type()
{
    return AST::Type::Array;
}

string ASTArray::Print()
{
    string s = "[ ";
    for (size_t i = 0; i < m_elements.size(); i++){
        s += m_elements[i]->Print();
        if (i != m_elements.size() - 1)
            s += " , ";
    }

    s += " ]";

    return s;
}



ASTAccessArray::ASTAccessArray(AST * array, AST * index)
: m_array(array) , m_index(index) {}

ASTAccessArray::~ASTAccessArray()
{
    delete m_array;
    delete m_index;
}

ASTValue * ASTAccessArray::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTAssignArray::ASTAssignArray(AST * array, AST * index, AST * value)
: m_array(array) , m_index(index) , m_value(value) 
{
}

ASTAssignArray::~ASTAssignArray()
{
    delete m_array;
    delete m_index;
    delete m_value;
}

ASTValue * ASTAssignArray::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTFunction::ASTFunction(const string & name, vector<string> & param,
            AST * body,const string & json)
: m_name(name) , m_param(param) , m_body(body) , m_json(json) 
{
}

ASTFunction::~ASTFunction()
{
    delete m_body;
}

ASTValue * ASTFunction::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTCallFunction::ASTCallFunction(const string & name, vector<AST*> & arg)
: m_name(name) , m_arg(arg) 
{
}

ASTCallFunction::~ASTCallFunction()
{
    for (size_t i = 0; i < m_arg.size(); i++)
        delete m_arg[i];
}

ASTValue * ASTCallFunction::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTPrint::ASTPrint(const string & format, vector<AST*> & arg)
: m_format(format) , m_arg(arg) 
{
}

ASTPrint::~ASTPrint()
{
    for (size_t i = 0; i < m_arg.size(); i++)
        delete m_arg[i];
}

ASTValue * ASTPrint::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTBlock::ASTBlock(vector<AST*> & stmt)
: m_stmt(stmt) 
{
}

ASTBlock::~ASTBlock()
{
    for (size_t i = 0; i < m_stmt.size(); i++)
        delete m_stmt[i];
}

ASTValue * ASTBlock::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTTop::ASTTop(vector<AST*> & stmt)
: m_stmt(stmt) 
{
}

ASTTop::~ASTTop()
{
    for (size_t i = 0; i < m_stmt.size(); i++)
        delete m_stmt[i];
}

ASTValue * ASTTop::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTLoop::ASTLoop(AST * cond, AST * body)
: m_cond(cond) , m_body(body) 
{
}

ASTLoop::~ASTLoop()
{
    delete m_cond;
    delete m_body;
}

ASTValue * ASTLoop::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTConditional::ASTConditional(AST * cond, AST * _if, AST * _else)
: m_cond(cond) , m_if(_if) , m_else(_else) 
{
}

ASTConditional::~ASTConditional()
{
    delete m_cond;
    delete m_if;
    delete m_else;
}

ASTValue * ASTConditional::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTObject::ASTObject(AST * extends, vector<AST*> & members)
: m_extends(extends) , m_members(members) 
{
}

ASTObject::ASTObject()
{
}

ASTObject::~ASTObject()
{
    delete m_extends;
    for (size_t i = 0; i < m_members.size(); i++)
        delete m_members[i];
}

ASTValue * ASTObject::Accept(Visitor & v)
{
    return v.visit(this);
}

AST::Type ASTObject::Type()
{
    return AST::Type::Object;
}

string ASTObject::Print()
{
    return "Object";
}



ASTAssignField::ASTAssignField(AST * object, const string & field, AST * value)
: m_object(object) , m_field(field) , m_value(value) 
{
}

ASTAssignField::~ASTAssignField()
{
    delete m_object;
    delete m_value;
}

ASTValue * ASTAssignField::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTAccessField::ASTAccessField(AST * object, const string & field)
: m_object(object) , m_field(field) 
{
}

ASTAccessField::~ASTAccessField()
{
    delete m_object;
}

ASTValue * ASTAccessField::Accept(Visitor & v)
{
    return v.visit(this);
}



ASTCallMethod::ASTCallMethod(AST * object, const string & name, vector<AST*> & arg)
: m_object(object) , m_name(name) , m_arg(arg) 
{
}

ASTCallMethod::~ASTCallMethod()
{
    delete m_object;
    for (size_t i = 0; i < m_arg.size(); i++)
        delete m_arg[i];
}

ASTValue * ASTCallMethod::Accept(Visitor & v)
{
    return v.visit(this);
}


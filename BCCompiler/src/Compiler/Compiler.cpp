#include "Compiler.hpp"
#include <variant>
#include <cstring>
#include "../Utils/utils.hpp"
using namespace std;

Compiler::Compiler() 
{
    m_counter_locals = 0;
    m_code = make_shared<vector<ins>>();
    m_frame = make_shared<frame>();
    m_globalFrame = m_frame;
    m_globalFrame->m_prev = nullptr;
    m_globalFrame->m_endOfScope = true;
    m_globalFrame->m_sizePrev = 0;
}

uint16_t Compiler::InsertConst(const string & str, constant con) 
{
    uint16_t pos;
    auto it = m_globalFrame->m_varMap.insert({str,m_constant_pool.size()});
    if (it.second)
    {
        pos = m_constant_pool.size();
        m_constant_pool.push_back(con);
    }
    else
    {
        pos = it.first->second;
    }
    return pos;
}

ASTValue * Compiler::visit(ASTInteger * integer) 
{
    string str = to_string((int)ProgramObject::Opcode::Integer);
    str += to_string(integer->m_value);
    uint16_t index = InsertConst(str,OInteger(integer->m_value));
    ILiteral lit;
    lit.m_index = index;
    m_code->push_back(lit);
    return nullptr;
}

ASTValue * Compiler::visit(ASTBoolean * boolean) 
{
    string str = to_string((int)ProgramObject::Opcode::Boolean);
    str += to_string(boolean->m_value);
    uint16_t index = InsertConst(str,OBoolean(boolean->m_value));
    ILiteral lit;
    lit.m_index = index;
    m_code->push_back(lit);
    return nullptr;
}

ASTValue * Compiler::visit(ASTNull * null) 
{
    string str = to_string((int)ProgramObject::Opcode::Null);
    uint16_t index = InsertConst(str,ONull());
    ILiteral lit;
    lit.m_index = index;
    m_code->push_back(lit);
    return nullptr;
}

ASTValue * Compiler::visit(ASTVariable * variable) 
{
    variable->m_value->Accept(*this);
    if (m_globalFrame == m_frame)
    {
        OString ostr;
        ostr.m_characters = variable->m_name;
        ostr.m_length = variable->m_name.size();
        uint16_t index = InsertConst(ostr.m_characters,ostr);
        OSlot slot;
        slot.m_name = index;
        m_constant_pool.push_back(slot);
        m_globals.push_back(m_constant_pool.size() - 1);
        ISet_Global set;
        set.m_index = index;
        m_code->push_back(set);
    }
    else
    {
        m_counter_locals++;
        auto it = m_frame->m_varMap.insert({variable->m_name,m_frame->m_varMap.size()+m_frame->m_sizePrev});
        if (it.second == false)
            throw "Error: Variable " + variable->m_name + " was already declared in current scope.";
        ISet_Local loc;
        loc.m_index = m_frame->m_varMap.size() + m_frame->m_sizePrev - 1;
        m_code->push_back(loc);
    }
    m_code->push_back(IDrop());

    return nullptr;
}

bool Compiler::FindName(const string & str, uint16_t & index) 
{
    shared_ptr<frame> cur = m_frame;
    do{
        if (cur == m_globalFrame)
            break;
        auto it = cur->m_varMap.find(str);
        if (it != cur->m_varMap.end())
        {
            index = it->second;
            return true;
        }
        if (cur->m_endOfScope)
            cur = nullptr;
        else
            cur = cur->m_prev;
    }while(cur);

    auto it = m_globalFrame->m_varMap.find(str);
    if (it == m_globalFrame->m_varMap.end())
        throw "Error: Variable " + str + " was not declared";
    index = it->second;
    return false;
}

template <typename T>
void Compiler::SetAndPush(T i, uint16_t index) 
{
    i.m_index = index;
    m_code->push_back(i);
}


ASTValue * Compiler::visit(ASTAccessVariable * accessVar) 
{
    uint16_t index;
    FindName(accessVar->m_name,index) 
        ? SetAndPush(IGet_Local(),index) : SetAndPush(IGet_Global(),index);
    return nullptr;
}

ASTValue * Compiler::visit(ASTAssignVariable * assignVar) 
{
    uint16_t index;
    assignVar->m_value->Accept(*this);
    FindName(assignVar->m_name,index) 
        ? SetAndPush(ISet_Local(),index) : SetAndPush(ISet_Global(),index);
    m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTFunction * fun) 
{
    m_codes.push_back(m_code);
    m_code = make_shared<vector<ins>>();

    OString ostr;
    ostr.m_characters = fun->m_name;
    ostr.m_length = fun->m_name.size();
    uint16_t index = InsertConst(ostr.m_characters,ostr);

    OMethod method;
    method.m_name = index;
    method.m_arguments = fun->m_param.size();


    shared_ptr<frame> now = make_shared<frame>();
    now->m_endOfScope = true;
    now->m_sizePrev = 0;
    now->m_prev = m_frame;
    m_frame = now;

    for (auto & x : fun->m_param)
        m_frame->m_varMap.insert({x,m_frame->m_varMap.size()});

    fun->m_body->Accept(*this);

    m_frame = m_frame->m_prev;

    ins instruct = ((*m_code)[m_code->size()-1]);
    if (holds_alternative<IDrop>(instruct))
    {
        m_code->pop_back();
    }

    m_code->push_back(IReturn());

    method.m_locals = m_counter_locals;
    m_counter_locals = 0;
    
    method.m_ins = m_code;
    method.m_length = m_code->size();

    m_constant_pool.push_back(method);
    m_globals.push_back(m_constant_pool.size() - 1);

    m_code = m_codes.back();
    m_codes.pop_back();
    return nullptr;
}

ASTValue * Compiler::visit(ASTCallFunction * callFun) 
{
    for (auto & x: callFun->m_arg)
    {
        x->Accept(*this);
    }

    uint16_t index;
    if (!FindName(callFun->m_name,index))
    {
        ICall_Function call;
        call.m_index = index;
        call.m_arguments = callFun->m_arg.size();
        m_code->push_back(call);
    }
    else
        throw "Error: function has to be global.";
    return nullptr;
}

ASTValue * Compiler::visit(ASTPrint * print) 
{
    for (auto & x : print->m_arg)
    {
        x->Accept(*this);
    }
    string str = to_string((int)ProgramObject::Opcode::String);
    str += print->m_format;
    OString ostr;
    ostr.m_characters = print->m_format;
    ostr.m_length = print->m_format.size();
    uint16_t index = InsertConst(str,ostr);
    IPrint p;
    p.m_arguments = print->m_arg.size();
    p.m_index = index;
    m_code->push_back(p);
    m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTBlock * block) 
{
    shared_ptr<frame> now = make_shared<frame>();
    now->m_endOfScope = false;
    if (m_frame != m_globalFrame)
    {
        now->m_sizePrev = m_frame->m_varMap.size() + m_frame->m_sizePrev;
    }
    else
    {
        now->m_sizePrev = 0;
    }
    now->m_prev = m_frame;
    m_frame = now;
    for (auto & x : block->m_stmt)
    {
        x->Accept(*this);
        if (x != block->m_stmt[block->m_stmt.size()-1])
        {
            ins instruct = ((*m_code)[m_code->size()-1]);
            if (holds_alternative<ILiteral>(instruct))
            {
                m_code->pop_back();
            }
        }
    }
    m_frame = m_frame->m_prev;
    return nullptr;
}

ASTValue * Compiler::visit(ASTTop * top) 
{
    string str = to_string((int)ProgramObject::Opcode::String);
    str += "λ:";
    OString ostr;
    ostr.m_characters = "λ:";
    ostr.m_length = ostr.m_characters.size();
    uint16_t index = InsertConst(str,ostr);

    for (uint32_t i = 0; i < top->m_stmt.size(); i++)
        top->m_stmt[i]->Accept(*this);

    OMethod method;
    method.m_name = index;
    method.m_arguments = 0;
    method.m_locals = m_counter_locals;
    m_counter_locals = 0;

    method.m_ins = m_code;
    method.m_length = m_code->size();

    m_constant_pool.push_back(method);
    
    m_entry = m_constant_pool.size() - 1;

    return nullptr;
}

ASTValue * Compiler::visit(ASTLoop * loop) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTConditional * cond) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTObject * obj) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTArray * arr) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTAssignArray * assignArr) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTAccessArray * accessArr) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTAssignField * assignFie) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTAccessField * accessFie) 
{
    return nullptr;
}

ASTValue * Compiler::visit(ASTCallMethod * call) 
{
    return nullptr;
}

void Compiler::reallocate(unsigned char * data,uint64_t size, uint64_t max)
{
    unsigned char * dataNew = new unsigned char[max];
    memcpy(dataNew,data,size);
    delete [] data;
    data = dataNew;
}

unsigned char * Compiler::Final(uint64_t & size) 
{
    size = 0;
    uint64_t max = 1000;
    unsigned char * code = new unsigned char[max];

    writeInt16_t(code,m_constant_pool.size());
    size += 2;

    for (auto x : m_constant_pool)
    {
        uint64_t potential = 12;
        if (OMethod * m = get_if<OMethod>(&x))
        {
            potential += (m->m_length * 4);
        }
        if (size + potential >= max)
        {
            max = (size + potential) * 2;
            reallocate(code,size,max);
        }
        uint32_t sizeO = 0;
        std::visit([&](auto && v){v.writeByteCode(code + size,sizeO);},x);
        size += sizeO;
    }

    uint64_t potential = size + 4 + (m_globals.size() * 2);
    if (potential >= max)
        reallocate(code,size,potential + 1);

    writeInt16_t(code + size, m_globals.size());
    size += 2;

    for (auto & x : m_globals)
    {
        writeInt16_t(code + size, x);
        size += 2;
    }
    writeInt16_t(code + size, m_entry);
    size += 2;
    return code;
}

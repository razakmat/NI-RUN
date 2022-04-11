#include "Compiler.hpp"
#include "../Utils/utils.hpp"

#include <cstring>

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

uint16_t Compiler::CreateStringToConst(const string & str)
{
    string strM = to_string((int)ProgramObject::Opcode::String) + str;
    OString ostr;
    ostr.m_characters = str;
    ostr.m_length = str.size();
    return InsertConst(strM,ostr);
}

ASTValue * Compiler::visit(ASTInteger * integer) 
{
    string str = to_string((int)ProgramObject::Opcode::Integer);
    str += to_string(integer->m_value);
    uint16_t index = InsertConst(str,OInteger(integer->m_value));
    ILiteral lit;
    lit.m_index = index;
    m_code->push_back(lit);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
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
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTNull * null) 
{
    string str = to_string((int)ProgramObject::Opcode::Null);
    uint16_t index = InsertConst(str,ONull());
    ILiteral lit;
    lit.m_index = index;
    m_code->push_back(lit);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTVariable * variable) 
{
    m_to_drop.push(false);
    variable->m_value->Accept(*this);
    m_to_drop.pop();
    if (m_globalFrame == m_frame)
    {
        uint16_t index;
        auto it = m_globalFrame->m_varMap.find(variable->m_name);
        if (it == m_globalFrame->m_varMap.end())
        {
            OString ostr;
            ostr.m_characters = variable->m_name;
            ostr.m_length = variable->m_name.size();
            uint16_t indexin = InsertConst(ostr.m_characters,ostr);
            OSlot slot;
            slot.m_name = indexin;
            m_constant_pool.push_back(slot);
            m_globals.push_back(m_constant_pool.size() - 1);
            index = indexin;
        }
        else
            index = it->second;
        ISet_Global set;
        set.m_index = index;
        m_code->push_back(set);
    }
    else
    {
        m_counter_locals++;
        auto it =m_frame->m_varMap.insert({variable->m_name,m_frame->m_varMap.size()+m_frame->m_sizePrev});
        if (it.second == false)
            throw "Error: Variable " + variable->m_name + " was already declared in current scope.";
        ISet_Local loc;
        loc.m_index = m_frame->m_varMap.size() + m_frame->m_sizePrev - 1;
        m_code->push_back(loc);
    }
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

bool Compiler::FindName(const string & str, uint16_t & index,bool global) 
{
    if (!global)
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
    }
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
    m_to_drop.push(false);
    assignVar->m_value->Accept(*this);
    m_to_drop.pop();

    uint16_t index;
    FindName(assignVar->m_name,index) 
        ? SetAndPush(ISet_Local(),index) : SetAndPush(ISet_Global(),index);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTFunction * fun) 
{
    m_codes.push_back(m_code);
    m_code = make_shared<vector<ins>>();

    uint16_t index;
    FindName(fun->m_name,index);

    OMethod method;
    method.m_name = index;
    method.m_arguments = fun->m_param.size();

    shared_ptr<frame> now = make_shared<frame>();
    now->m_endOfScope = true;
    now->m_sizePrev = 0;
    now->m_prev = m_frame;
    m_frame = now;

    if (m_method){
        m_frame->m_varMap.insert({"this",0});
        method.m_arguments++;
    }
    for (auto & x : fun->m_param)
        m_frame->m_varMap.insert({x,m_frame->m_varMap.size()});

    m_vector_locals.push_back(m_counter_locals);

    m_to_drop.push(false);
    fun->m_body->Accept(*this);
    m_to_drop.pop();

    m_frame = m_frame->m_prev;

    ins instruct = ((*m_code)[m_code->size()-1]);
    if (holds_alternative<IDrop>(instruct))
    {
        m_code->pop_back();
    }

    m_code->push_back(IReturn());

    method.m_locals = m_counter_locals;
    m_counter_locals = m_vector_locals.back();
    m_vector_locals.pop_back();
    
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
    m_to_drop.push(false);
    for (auto & x: callFun->m_arg)
    {
        x->Accept(*this);
    }
    m_to_drop.pop();

    uint16_t index;
    if (!FindName(callFun->m_name,index))
    {
        ICall_Function call;
        call.m_index = index;
        call.m_arguments = callFun->m_arg.size();
        m_code->push_back(call);
    }
    else
        throw (string)"Error: function has to be global.";
    return nullptr;
}

ASTValue * Compiler::visit(ASTPrint * print) 
{
    m_to_drop.push(false);
    for (auto & x : print->m_arg)
    {
        x->Accept(*this);
    }
    m_to_drop.pop();

    uint16_t index = CreateStringToConst(print->m_format);
    IPrint p;
    p.m_arguments = print->m_arg.size();
    p.m_index = index;
    m_code->push_back(p);
    if (m_to_drop.top())
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

    m_to_drop.push(true);
    for (auto & x : block->m_stmt)
    {
        if (x == block->m_stmt[block->m_stmt.size()-1])
            m_to_drop.pop();
        x->Accept(*this);
    }
    m_frame = m_frame->m_prev;
    return nullptr;
}

void Compiler::DefineGlobalFunction(ASTFunction * fun)
{
    OString ostr;
    ostr.m_characters = fun->m_name;
    ostr.m_length = fun->m_name.size();
    InsertConst(ostr.m_characters,ostr);
}


void Compiler::DefineGlobalVar(ASTVariable * var)
{
    ASTNull * null = nullptr;
    visit(null);
    OString ostr;
    ostr.m_characters = var->m_name;
    ostr.m_length = var->m_name.size();
    uint16_t index = InsertConst(ostr.m_characters,ostr);
    OSlot slot;
    slot.m_name = index;
    m_constant_pool.push_back(slot);
    m_globals.push_back(m_constant_pool.size() - 1);
    ISet_Global set;
    set.m_index = index;
    m_code->push_back(set);
    m_code->push_back(IDrop());
}

ASTValue * Compiler::visit(ASTTop * top) 
{
    uint16_t index = CreateStringToConst(m_nameOfmain);

    m_to_drop.push(false);

    for (uint32_t i = 0; i < top->m_stmt.size(); i++)
    {
        if (ASTVariable * var = dynamic_cast<ASTVariable*>(top->m_stmt[i]))
            DefineGlobalVar(var);
        else if (ASTFunction * fun = dynamic_cast<ASTFunction*>(top->m_stmt[i]))
            DefineGlobalFunction(fun);
    }
    m_to_drop.pop();
    m_to_drop.push(true);
    
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
    uint16_t indexStart = CreateStringToConst(to_string(m_counterLabel++));
    uint16_t indexCond = CreateStringToConst(to_string(m_counterLabel++));
    
    IJump jump;
    jump.m_index = indexCond;
    m_code->push_back(jump);

    m_to_drop.push(true);

    ILabel label;
    label.m_index = indexStart;
    m_code->push_back(label);
    loop->m_body->Accept(*this);

    m_to_drop.pop();

    m_to_drop.push(false);

    label.m_index = indexCond;
    m_code->push_back(label);
    loop->m_cond->Accept(*this);

    m_to_drop.pop();

    IBranch branch;
    branch.m_index = indexStart;
    m_code->push_back(branch);
    return nullptr;
}

ASTValue * Compiler::visit(ASTConditional * cond) 
{
    uint16_t indexThen = CreateStringToConst(to_string(m_counterLabel++));
    uint16_t indexEnd = CreateStringToConst(to_string(m_counterLabel++));

    m_to_drop.push(false);

    cond->m_cond->Accept(*this);
    IBranch branch;
    branch.m_index = indexThen;
    m_code->push_back(branch);

    m_to_drop.pop();

    cond->m_else->Accept(*this);
    IJump jump;
    jump.m_index = indexEnd;
    m_code->push_back(jump);

    ILabel label;
    label.m_index = indexThen;
    m_code->push_back(label);
    cond->m_if->Accept(*this);

    label.m_index = indexEnd;
    m_code->push_back(label);
    return nullptr;
}

ASTValue * Compiler::visit(ASTObject * obj) 
{
    OClass c;
    c.m_length = obj->m_members.size();

    m_to_drop.push(false);
    obj->m_extends->Accept(*this);

    for (auto & x : obj->m_members)
    {
        if (ASTVariable * var = dynamic_cast<ASTVariable*>(x))
        {
            OString ostr;
            ostr.m_characters = var->m_name;
            ostr.m_length = var->m_name.size();
            uint16_t index = InsertConst(ostr.m_characters,ostr);
            OSlot slot;
            slot.m_name = index;
            m_constant_pool.push_back(slot);
            c.m_members.push_back(m_constant_pool.size() - 1);

            var->m_value->Accept(*this);
        }
        else if (ASTFunction * fun = dynamic_cast<ASTFunction*>(x))
        {
            DefineGlobalFunction(fun);
            m_method = true;
            visit(fun);
            m_method = false;

            c.m_members.push_back(m_globals.back());
            m_globals.pop_back();
        }
        else
            throw (string)"Error : Class can hold only fields or methods.";
    }
    m_to_drop.pop();
    
    m_constant_pool.push_back(c);
    IObject object;
    object.m_index = m_constant_pool.size() - 1;
    m_code->push_back(object);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTAssignField * assignFie) 
{
    m_to_drop.push(false);
    assignFie->m_object->Accept(*this);
    assignFie->m_value->Accept(*this);
    m_to_drop.pop();

    ISet_Field set;
    FindName(assignFie->m_field,set.m_index,true);
    m_code->push_back(set);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTAccessField * accessFie) 
{
    m_to_drop.push(false);
    accessFie->m_object->Accept(*this);
    m_to_drop.pop();

    IGet_Field get;
    FindName(accessFie->m_field,get.m_index,true);
    m_code->push_back(get);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
    return nullptr;
}

ASTValue * Compiler::visit(ASTCallMethod * call) 
{
    m_to_drop.push(false);
    call->m_object->Accept(*this);
    for (auto & x : call->m_arg)
        x->Accept(*this);
    m_to_drop.pop();

    ICall_Method c;
    c.m_arguments = call->m_arg.size() + 1;

    OString ostr;
    ostr.m_characters = call->m_name;
    ostr.m_length = call->m_name.size();
    c.m_index = InsertConst(call->m_name, ostr);

    m_code->push_back(c);
    if (m_to_drop.top())
        m_code->push_back(IDrop());
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



void Compiler::reallocate(unsigned char *& data,uint64_t size, uint64_t max)
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

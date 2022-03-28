#include "Dispatch.hpp"

Dispatch::Dispatch(vector<uint32_t> & args, FMLVM * vm, const string & name, uint32_t point_rec)
: m_args(args) , m_vm(vm), m_name(name), m_point_rec(point_rec)
{
    
}

void Dispatch::RetBool(bool value) 
{
    constant con = OBoolean(value);
    m_vm->m_op_stack.Push(m_vm->m_heap.AssignLiteral(con));
}


void Dispatch::RetInt(int32_t value) 
{
    constant con = OInteger(value);
    m_vm->m_op_stack.Push(m_vm->m_heap.AssignLiteral(con));
}


void Dispatch::operator ()(ROBoolean & boolean) 
{
    if (m_args.size() != 1)
        throw "Error: wrong number of arguments in methodCall for boolean.";
    runObj arg = m_vm->m_heap.GetRunObject(m_args[0]);
    ROBoolean * RObj = get_if<ROBoolean>(&arg);
    if (m_name == "==" || m_name == "eq")
    {
        RObj ? RetBool(boolean.m_value == RObj->m_value) : RetBool(false);
        return;
    }
    else if (m_name == "!=" || m_name == "neq")
    {
        RObj ? RetBool(boolean.m_value != RObj->m_value) : RetBool(true);
        return;
    }
    if (RObj == nullptr)
        throw "Error: Wrong method call.";
    if (m_name == "&" || m_name == "and")
        RetBool(boolean.m_value && RObj->m_value);
    else if (m_name == "|" || m_name == "or")
        RetBool(boolean.m_value || RObj->m_value);
    else
        throw "Error: Wrong method call.";
}

void Dispatch::operator ()(ROInteger & integer) 
{
    if (m_args.size() != 1)
        throw "Error: wrong number of arguments in methodCall for integer.";
    runObj arg = m_vm->m_heap.GetRunObject(m_args[0]);
    ROInteger * RObj = get_if<ROInteger>(&arg);
    if (m_name == "==" || m_name == "eq")
    {
        RObj ? RetBool(integer.m_value == RObj->m_value) : RetBool(false);
        return;
    }
    else if (m_name == "!=" || m_name == "neq")
    {
        RObj ? RetBool(integer.m_value != RObj->m_value) : RetBool(true);
        return;
    }
    if (RObj == nullptr)
        throw "Error: Wrong method call.";
    if (m_name == "+" || m_name == "add")
        RetInt(integer.m_value + RObj->m_value);
    else if (m_name == "-" || m_name == "sub")
        RetInt(integer.m_value - RObj->m_value);
    else if (m_name == "*" || m_name == "mul")
        RetInt(integer.m_value * RObj->m_value);
    else if (m_name == "/" || m_name == "div")
        RetInt(integer.m_value / RObj->m_value);
    else if (m_name == "%" || m_name == "mod")
        RetInt(integer.m_value % RObj->m_value);
    else if (m_name == "<=" || m_name == "le")
        RetBool(integer.m_value <= RObj->m_value);
    else if (m_name == ">=" || m_name == "ge")
        RetBool(integer.m_value >= RObj->m_value);
    else if (m_name == "<" || m_name == "lt")
        RetBool(integer.m_value < RObj->m_value);
    else if (m_name == ">" || m_name == "gt")
        RetBool(integer.m_value > RObj->m_value);
    else
        throw "Error: Wrong method call.";
}

void Dispatch::operator ()(RONull & nul) 
{
    if (m_args.size() != 1)
        throw "Error: wrong number of arguments in methodCall for null.";
    runObj arg = m_vm->m_heap.GetRunObject(m_args[0]);
    RONull * RObj = get_if<RONull>(&arg);
    if (m_name == "==" || m_name == "eq")
    {
        RObj ? RetBool(true) : RetBool(false);
    }
    else if (m_name == "!=" || m_name == "neq")
    {
        RObj ? RetBool(false) : RetBool(true);
    }
    else
        throw "Error: Wrong method call.";
}

void Dispatch::operator ()(ROArray & arr) 
{
    if (m_args.size() < 1)
        throw "Error: wrong number of arguments in methodCall for array.";
    runObj arg = m_vm->m_heap.GetRunObject(m_args[0]);
    ROInteger * RObj = get_if<ROInteger>(&arg);
    if (RObj == nullptr)
        throw "Error: Index for array method call has to be integer.";
    if (RObj->m_value < 0)
        throw "Error: Index for array method call cannot be negative.";
    if (m_name == "get")
    {
        if (m_args.size() != 1)
            throw "Error: wrong number of arguments in methodCall 'get'.";
        m_vm->m_op_stack.Push(m_vm->m_heap.ArrayGet(m_point_rec,RObj->m_value));
    }
    else if (m_name == "set")
    {
        if (m_args.size() != 2)
            throw "Error: wrong number of arguments in methodCall 'set'.";
        m_vm->m_heap.ArraySet(m_point_rec,RObj->m_value,m_args[1]);
        m_vm->m_op_stack.Push(m_args[1]);
    }
    else
        throw "Error: Wrong method call for array.";
}

void Dispatch::operator ()(ROObject & obj) 
{
    throw "Error : Some unknown Runtime Object!";
}

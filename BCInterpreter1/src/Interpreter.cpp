#include "Interpreter.hpp"
#include "Instruction.hpp"
#include <variant>
#include <vector>
#include <iostream>
#include "OpStack.hpp"

Interpreter::Interpreter(FMLVM * vm)
: m_vm(vm)
{
    OMethod & method = get<OMethod>(m_vm->m_constant_pool[m_vm->m_entry_point]);
    m_PC = method.m_start;
    m_vm->m_frame_stack.PushStack(method.m_arguments + method.m_locals);
}

Interpreter::~Interpreter()
{
    delete m_vm;   
}

void Interpreter::Run()
{
    
    while (m_PC < m_vm->m_instructions.size())
    {
        ins & instruction = m_vm->m_instructions[m_PC];

        visit(*this,instruction);
    }
}

void Interpreter::operator()(ILiteral & literal)
{
    constant & obj = m_vm->m_constant_pool[literal.m_index];

    uint32_t pointer = m_vm->m_heap.AssignLiteral(obj);
    m_vm->m_op_stack.Push(pointer);
    m_PC++;
}

void Interpreter::operator()(IDrop & drop)
{
    m_vm->m_op_stack.Pop();
    m_PC++;
}

void Interpreter::operator()(IPrint & print)
{
    constant & obj = m_vm->m_constant_pool[print.m_index];
    OString & str = get<OString>(obj);
    print_out(print.m_arguments,str.m_characters);
    m_vm->m_op_stack.Push(0);
    m_PC++;
}

void Interpreter::operator()(IJump & jump)
{
    constant & obj = m_vm->m_constant_pool[jump.m_index];
    OString & str = get<OString>(obj);
    auto res = m_vm->m_labels.find(str.m_characters);
    if (res == m_vm->m_labels.end())
        throw "Label was not found in map.";
    m_PC = res->second;
}

void Interpreter::operator()(IBranch & branch)
{
    uint32_t pointer = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    runObj obj = m_vm->m_heap.GetRunObject(pointer);

    if (holds_alternative<RONull>(obj))
    {
        m_PC++;
        return;
    }
    if (holds_alternative<ROBoolean>(obj))
    {
        ROBoolean b = get<ROBoolean>(obj);
        if (b.m_value == false)
        {
            m_PC++;
            return;
        }
    }
    IJump jump{branch.m_index};
    this->operator()(jump);
}

void Interpreter::operator()(ILabel & label) 
{
    m_PC++;
}

template <typename A>
void Interpreter::operator()(A & instruction)
{
    throw "Something wrong!";
}

void Interpreter::operator()(IGet_Local & getLocal) 
{
    uint32_t index = m_vm->m_frame_stack.GetLocal(getLocal.m_index);
    m_vm->m_op_stack.Push(index);
    m_PC++;
}

void Interpreter::operator()(ISet_Local & setLocal) 
{
    uint32_t pointer = m_vm->m_op_stack.Get();
    m_vm->m_frame_stack.SetLocal(setLocal.m_index,pointer);
    m_PC++;
}

void Interpreter::operator()(IGet_Global & getGlobal) 
{
    OString & obj = get<OString>(m_vm->m_constant_pool[getGlobal.m_index]);
    uint32_t index = m_vm->m_frame_stack.GetGlobal(obj.m_characters);
    m_vm->m_op_stack.Push(index);
    m_PC++;
}

void Interpreter::operator()(ISet_Global & setGlobal) 
{
    uint32_t pointer = m_vm->m_op_stack.Get();
    OString & obj = get<OString>(m_vm->m_constant_pool[setGlobal.m_index]);
    m_vm->m_frame_stack.SetGlobal(obj.m_characters,pointer);
    m_PC++;
}

void Interpreter::operator()(ICall_Function & call) 
{
    OString & obj = get<OString>(m_vm->m_constant_pool[call.m_index]);
    uint32_t index = m_vm->m_frame_stack.GetGlobal(obj.m_characters);
    OMethod & method= get<OMethod>(m_vm->m_constant_pool[index]);

    m_vm->m_frame_stack.PushStack(method.m_arguments + method.m_locals);
    for (int i = method.m_arguments - 1; i >= 0; i--)
    {
        uint32_t arg = m_vm->m_op_stack.Get();
        m_vm->m_op_stack.Pop();
        m_vm->m_frame_stack.SetLocal(i,arg);
    }
    for (int i = method.m_arguments; i < method.m_arguments + method.m_locals; i++)
        m_vm->m_frame_stack.SetLocal(i,0);
    m_PC++;
    m_vm->m_frame_stack.SetReturn(m_PC);
    m_PC = method.m_start;
}

void Interpreter::operator()(IReturn & ret) 
{
    uint32_t retAddress = m_vm->m_frame_stack.GetReturn();
    m_PC = retAddress;
    m_vm->m_frame_stack.PopStack();
}

void Interpreter::print_out(int16_t args,const string & str)
{
    for (uint32_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '~')
        {
            if (args <= 0)
                throw "Error : Wrong number of arguments in print.";
            runObj obj = m_vm->m_heap.GetRunObject(m_vm->m_op_stack.Get(args--));
            if (ROInteger * oInt = get_if<ROInteger>(&obj))
            {
                cout << oInt->m_value;
            }
            else if (ROBoolean * oBool = get_if<ROBoolean>(&obj))
            {
                cout << boolalpha << oBool->m_value;
            }
            else if (get_if<RONull>(&obj))
            {
                cout << "null";
            }
            else if (get_if<ROArray>(&obj))
            {
                cout << "Array";
            }
            else if (get_if<ROObject>(&obj))
            {
                cout << "Object";
            }
            else
                throw "Error within print!";
        }
        else if (str[i] == '\\')
        {
            if (++i == str.size())
                throw "Error : Wrong format in print.";
            switch (str[i]){
                case '~':
                    cout << '~';
                    break;
                case 'n':
                    cout << endl;
                    break;
                case '"':
                    cout << "\"";
                    break;
                case 'r':
                    cout << "\r";
                    break;
                case 't':
                    cout << "\t";
                    break;
                case '\\':
                    cout << "\\";
                default:
                    throw "Error : Wrong symbol after backslash in print.";
            }
        }
        else
            cout << str[i];
    }
}
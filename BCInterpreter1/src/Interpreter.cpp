#include "Interpreter.hpp"
#include "Instruction.hpp"
#include <variant>
#include <vector>
#include <iostream>
#include "OpStack.hpp"
#include "Dispatch.hpp"

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
        if (m_PC == 2539)
        {
            m_PC = 2539;
        }
        ins & instruction = m_vm->m_instructions[m_PC];

        visit(*this,instruction);
    }
}

void Interpreter::operator()(ILiteral & literal)
{
    constant & obj = m_vm->m_constant_pool[literal.m_index];

    uint64_t pointer = m_vm->m_heap.AssignLiteral(obj);
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
    uint64_t pointer = m_vm->m_op_stack.Get();
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
    throw "Error : Some unknown instruction!";
}

void Interpreter::operator()(IGet_Local & getLocal) 
{
    uint64_t index = m_vm->m_frame_stack.GetLocal(getLocal.m_index);
    m_vm->m_op_stack.Push(index);
    m_PC++;
}

void Interpreter::operator()(ISet_Local & setLocal) 
{
    uint64_t pointer = m_vm->m_op_stack.Get();
    m_vm->m_frame_stack.SetLocal(setLocal.m_index,pointer);
    m_PC++;
}

void Interpreter::operator()(IGet_Global & getGlobal) 
{
    OString & obj = get<OString>(m_vm->m_constant_pool[getGlobal.m_index]);
    uint64_t index = m_vm->m_frame_stack.GetGlobal(obj.m_characters);
    m_vm->m_op_stack.Push(index);
    m_PC++;
}

void Interpreter::operator()(ISet_Global & setGlobal) 
{
    uint64_t pointer = m_vm->m_op_stack.Get();
    OString & obj = get<OString>(m_vm->m_constant_pool[setGlobal.m_index]);
    m_vm->m_frame_stack.SetGlobal(obj.m_characters,pointer);
    m_PC++;
}

void Interpreter::operator()(ICall_Function & call) 
{
    OString & obj = get<OString>(m_vm->m_constant_pool[call.m_index]);
    uint64_t index = m_vm->m_frame_stack.GetGlobal(obj.m_characters);
    OMethod & method= get<OMethod>(m_vm->m_constant_pool[index]);

    m_vm->m_frame_stack.PushStack(method.m_arguments + method.m_locals);
    for (int i = method.m_arguments - 1; i >= 0; i--)
    {
        uint64_t arg = m_vm->m_op_stack.Get();
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

void Interpreter::operator()(IArray & arr) 
{
    uint64_t init = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    uint64_t size = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    runObj obj = m_vm->m_heap.GetRunObject(size);
    if (!holds_alternative<ROInteger>(obj))
        throw "Error : Array size has to be integer.";
    ROInteger num = get<ROInteger>(obj);
    if (num.m_value < 0)
        throw "Error : Size of array cannot be negative.";
    uint64_t pointer = m_vm->m_heap.AssignArray(num.m_value,init);
    m_vm->m_op_stack.Push(pointer);
    m_PC++;
}

void Interpreter::operator()(IObject & obj) 
{
    OClass & Oclass = get<OClass>(m_vm->m_constant_pool[obj.m_index]);
    vector<OMethod*> methods;
    ROObject ROobj;
    for (int16_t i = Oclass.m_length - 1; i >= 0; i--)
    {
        constant & cons = m_vm->m_constant_pool[Oclass.m_members[i]];
        if (OSlot * oSlot = get_if<OSlot>(&cons))
        {
            OString & name = get<OString>(m_vm->m_constant_pool[oSlot->m_name]);
            ROobj.m_fields.insert({name.m_characters,m_vm->m_op_stack.Get()});
            m_vm->m_op_stack.Pop();
        }
        if (OMethod * oMethod = get_if<OMethod>(&cons))
        {
            OString & name = get<OString>(m_vm->m_constant_pool[oMethod->m_name]);
            ROobj.m_methods.insert({name.m_characters,Oclass.m_members[i]});
        }
    }
    ROobj.m_parent = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    m_vm->m_op_stack.Push(m_vm->m_heap.AssignObject(ROobj));
    m_PC++;
}

void Interpreter::operator()(IGet_Field & getField) 
{
    OString & str = get<OString>(m_vm->m_constant_pool[getField.m_index]);
    uint64_t pointer = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    pointer = m_vm->m_heap.GetSetField(pointer,str.m_characters);
    m_vm->m_op_stack.Push(pointer);
    m_PC++;    
}

void Interpreter::operator()(ISet_Field & setField) 
{
    OString & str = get<OString>(m_vm->m_constant_pool[setField.m_index]);
    uint64_t value = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    uint64_t object = m_vm->m_op_stack.Get();
    m_vm->m_op_stack.Pop();
    m_vm->m_heap.GetSetField(object,str.m_characters,value,false);
    m_vm->m_op_stack.Push(value);
    m_PC++;
}

void Interpreter::operator()(ICall_Method & callMethod) 
{
    OString & str = get<OString>(m_vm->m_constant_pool[callMethod.m_index]);
    vector<uint64_t> args;
    for (uint8_t i = callMethod.m_arguments - 1; i > 0; i--)
    {
        uint64_t arg = m_vm->m_op_stack.Get(i);
        args.push_back(arg);
    }
    m_vm->m_op_stack.Pop(callMethod.m_arguments - 1);
    uint64_t point_rec = m_vm->m_op_stack.Get();
    runObj receiver = m_vm->m_heap.GetRunObject(point_rec);
    m_vm->m_op_stack.Pop();

    Dispatch disp(args,m_vm,str.m_characters,point_rec);

    while (true)
    {
        if (ROObject * obj = get_if<ROObject>(&receiver))
        {
            uint16_t const_pointer = m_vm->m_heap.GetMethod(point_rec,str.m_characters);
            if (const_pointer == 0)
            {
                point_rec = obj->m_parent;
                receiver = m_vm->m_heap.GetRunObject(obj->m_parent);
                continue;
            }
            OMethod & method = get<OMethod>(m_vm->m_constant_pool[const_pointer]);
            m_vm->m_frame_stack.PushStack(method.m_arguments + method.m_locals + 1);
            m_vm->m_frame_stack.SetLocal(0,point_rec);
            for (int i = 1; i < method.m_arguments; i++)
            {
                m_vm->m_frame_stack.SetLocal(i,args[i - 1]);
            }
            for (int i = method.m_arguments; i < method.m_arguments + method.m_locals; i++)
                m_vm->m_frame_stack.SetLocal(i + 1,0);
            m_PC++;
            m_vm->m_frame_stack.SetReturn(m_PC);
            m_PC = method.m_start;
            break;
        }
        else
        {
            visit(disp,receiver);
            m_PC++;
            break;
        }
    }
}

void Interpreter::print_out(int16_t args,const string & str)
{
    uint16_t num_args = args;
    for (uint32_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '~')
        {
            if (args <= 0)
                throw "Error : Wrong number of arguments in print.";
            cout << m_vm->m_heap.GetAsString(m_vm->m_op_stack.Get(args--));
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
    m_vm->m_op_stack.Pop(num_args);
}
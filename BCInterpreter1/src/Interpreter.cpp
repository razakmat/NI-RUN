#include "Interpreter.hpp"
#include "Instruction.hpp"
#include <variant>
#include <vector>
#include <iostream>

Interpreter::Interpreter(FMLVM * vm)
: m_vm(vm)
{
    m_PC = get<OMethod>(m_vm->m_constant_pool[m_vm->m_entry_point]).m_start;
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
    m_vm->m_op_stack.push(pointer);
    m_PC++;
}

void Interpreter::operator()(IDrop & drop)
{
    m_vm->m_op_stack.pop();
    m_PC++;
}

void Interpreter::operator()(IPrint & print)
{
    constant & obj = m_vm->m_constant_pool[print.m_index];
    OString & str = get<OString>(obj);
    vector<uint32_t> args;
    for (uint8_t i = 0; i < print.m_arguments; i++){
        args.push_back(m_vm->m_op_stack.top());
        m_vm->m_op_stack.pop();
    }
    print_out(args,str.m_characters);
    m_vm->m_op_stack.push(0);
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
    uint32_t pointer = m_vm->m_op_stack.top();
    m_vm->m_op_stack.pop();
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

template <typename A>
void Interpreter::operator()(A & instruction)
{
    throw "Something wrong!";
}

void Interpreter::print_out(vector<uint32_t> & args,const string & str)
{
    int16_t cur_arg = args.size() - 1;
    for (uint32_t i = 0; i < str.length(); i++)
    {
        if (str[i] == '~')
        {
            if (cur_arg < 0)
                throw "Error : Wrong number of arguments in print.";
            runObj obj = m_vm->m_heap.GetRunObject(args[cur_arg--]);
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
#include "OpStack.hpp"


uint64_t OpStack::Get(int i) 
{
    int index = m_stack.size() - i;
    if (index < 0)
        throw "Error: not enought elements in operand stack.";
    return m_stack[index];       
}

void OpStack::Pop(int i) 
{
    for (int j = 0; j < i; j++)
        m_stack.pop_back();
}

void OpStack::Push(uint64_t i) 
{
    m_stack.push_back(i);
}

#include "FrameStack.hpp"
#include <unordered_map>

FrameStack::FrameStack()
{
}

FrameStack::~FrameStack() 
{
    while (m_local != nullptr)
    {
        Frame * cur = m_local;
        m_local = m_local->m_prev;
        delete cur; 
    }
}

void FrameStack::PushStack(int i) 
{
    Frame * cur = new Frame();
    cur->m_locals.reserve(i);
    cur->m_prev = m_local;
    m_local = cur;
}

void FrameStack::PopStack() 
{
    Frame * cur = m_local;
    m_local = m_local->m_prev;
    delete cur;
}

void FrameStack::SetLocal(uint16_t index, uint64_t pointer) 
{
    m_local->m_locals[index] = pointer;
}

void FrameStack::SetReturn(uint32_t ret) 
{
    m_local->m_ret = ret;
}

uint32_t FrameStack::GetReturn() 
{
    return m_local->m_ret;
}

uint64_t FrameStack::GetLocal(uint16_t index) 
{
    return m_local->m_locals[index];
}

void FrameStack::SetGlobal(const string & name, uint64_t pointer) 
{
    auto it = m_global.m_GlobalMap.find(name);
    if (it == m_global.m_GlobalMap.end())
        throw "Error : Global variable " + name + " was not found.";
    it->second = pointer;
}

uint64_t FrameStack::GetGlobal(const string & name) 
{
    auto it = m_global.m_GlobalMap.find(name);
    if (it == m_global.m_GlobalMap.end())
        throw "Error : Global variable " + name + " was not found.";
    return it->second;
}

void FrameStack::InsertGlobal(const string & name, uint64_t index) 
{
    m_global.m_GlobalMap.insert({name,index});
}
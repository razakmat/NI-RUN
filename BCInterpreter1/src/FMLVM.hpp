#ifndef __FMLVM_H__
#define __FMLVM_H__

#include <vector>
#include <stack>
#include <unordered_map>
#include "ProgramObject.hpp"
#include "Instruction.hpp"
#include "Heap.hpp"
#include "FrameStack.hpp"

using namespace std;

class FMLVM
{
    public:
        vector<constant> m_constant_pool;
        vector<ins> m_instructions;
        vector<uint16_t> m_globals;
        uint16_t m_entry_point;
        Heap m_heap;
        stack<uint32_t> m_op_stack;
        FrameStack m_frame_stack;
        unordered_map<string,uint32_t> m_labels;
};


#endif // __FMLVM_H__
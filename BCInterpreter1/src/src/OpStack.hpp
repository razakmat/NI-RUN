#ifndef __OPSTACK_H__
#define __OPSTACK_H__

#include <vector>
#include <cstdint>

using namespace std;

class OpStack
{
    public:
        uint32_t Get(int i = 1);
        void Pop(int i = 1);
        void Push(uint32_t i);
    private:
    vector<uint32_t> m_stack;
};





#endif // __OPSTACK_H__
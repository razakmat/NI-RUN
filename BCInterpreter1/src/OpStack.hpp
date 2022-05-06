#ifndef __OPSTACK_H__
#define __OPSTACK_H__

#include <vector>
#include <cstdint>
#include <stack>

using namespace std;

class OpStack
{
    public:
        uint64_t Get(int i = 1);
        void Pop(int i = 1);
        void Push(uint64_t i);
        void GetRoots(stack<uint64_t> & roots);
    private:
    vector<uint64_t> m_stack;
};





#endif // __OPSTACK_H__
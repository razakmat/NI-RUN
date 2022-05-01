#ifndef __FRAMESTACK_H__
#define __FRAMESTACK_H__

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

using namespace std;

class FrameStack
{
    public:
        FrameStack();
        ~FrameStack();
        void SetGlobalFrame(int i);
        void PushStack(int i);
        void PopStack();
        void SetLocal(uint16_t index, uint64_t pointer);
        void SetReturn(uint32_t ret);
        uint32_t GetReturn();
        uint64_t GetLocal(uint16_t index);
        void SetGlobal(const string & name, uint64_t pointer);
        uint64_t GetGlobal(const string & name);
        void InsertGlobal(const string & name, uint64_t index);
    private:
        struct Frame{
            vector<uint64_t> m_locals;
            uint32_t m_ret;
            Frame * m_prev = nullptr;
        };
        struct GlobalFrame{
            unordered_map<string,uint64_t> m_GlobalMap;
        };        
        GlobalFrame m_global;
        Frame * m_local = nullptr;

};




#endif // __FRAMESTACK_H__
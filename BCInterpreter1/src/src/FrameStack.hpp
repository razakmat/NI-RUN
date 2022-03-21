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
        void SetLocal(uint32_t index, uint32_t pointer);
        void SetReturn(uint32_t ret);
        uint32_t GetReturn();
        uint32_t GetLocal(uint32_t index);
        void SetGlobal(const string & name, uint32_t pointer);
        uint32_t GetGlobal(const string & name);
        void InsertGlobal(const string & name, uint32_t index);
    private:
        struct Frame{
            vector<uint16_t> m_locals;
            uint32_t m_ret;
            Frame * m_prev = nullptr;
        };
        struct GlobalFrame{
            unordered_map<string,uint32_t> m_GlobalMap;
        };        
        GlobalFrame m_global;
        Frame * m_local = nullptr;

};




#endif // __FRAMESTACK_H__
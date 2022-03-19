#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include "ProgramObject.hpp"
#include "RuntimeObject.hpp"

class Heap
{
    public:
        Heap();
        ~Heap();
        uint64_t AssignLiteral(constant & obj);
        runObj GetRunObject(uint32_t index);
        void operator ()(OInteger & integer);
        void operator ()(OBoolean & boolean);
        void operator ()(ONull & nul);
        template<typename A>
        void operator ()(A & obj);
    private:
        void Reallocate(const uint64_t size);
        vector<uint32_t> m_index;
        unsigned char * m_data;
        uint64_t m_capacity;
        uint64_t m_size;
};



#endif // __HEAP_H__
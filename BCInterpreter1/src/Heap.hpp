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
        uint64_t AssignArray(uint32_t size, uint64_t element);
        uint64_t AssignObject(ROObject & obj);
        runObj GetRunObject(uint64_t index);
        uint64_t GetSetField(uint64_t index, const string & name,uint64_t value = 0, bool get = true);
        uint64_t ArrayGet(uint64_t index, uint32_t pos);
        void ArraySet(uint64_t index, uint32_t pos, uint64_t value);
        uint16_t GetMethod(uint64_t index,const string & name);
        const string GetAsString(uint64_t index); 
        void operator ()(OInteger & integer);
        void operator ()(OBoolean & boolean);
        void operator ()(ONull & nul);

        template<typename A>
        void operator ()(A & obj);
    private:
        void Reallocate(const uint64_t size);
        vector<uint64_t> m_index;
        unsigned char * m_data;
        uint64_t m_capacity;
        uint64_t m_size;
        uint64_t m_ret;
};



#endif // __HEAP_H__
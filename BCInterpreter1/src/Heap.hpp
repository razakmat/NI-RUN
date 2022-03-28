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
        uint32_t AssignLiteral(constant & obj);
        uint32_t AssignArray(uint32_t size, uint32_t element);
        uint32_t AssignObject(ROObject & obj);
        runObj GetRunObject(uint32_t index);
        uint32_t GetSetField(uint32_t index, const string & name,uint32_t value = 0, bool get = true);
        uint32_t ArrayGet(uint32_t index, uint32_t pos);
        void ArraySet(uint32_t index, uint32_t pos, uint32_t value);
        uint16_t GetMethod(uint32_t index,const string & name);
        const string GetAsString(uint32_t index); 
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
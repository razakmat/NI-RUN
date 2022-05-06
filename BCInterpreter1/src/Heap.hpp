#ifndef __HEAP_H__
#define __HEAP_H__

#include <vector>
#include "ProgramObject.hpp"
#include "RuntimeObject.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <stack>
#include <set>
#include <functional>

class Interpreter;
class Heap
{
    public:
        Heap(int heap_size = 0);
        ~Heap();
        uint64_t AssignLiteral(constant & obj);
        uint64_t AssignArray(uint32_t size, uint64_t element, Interpreter * inter);
        uint64_t AssignObject(ROObject & obj, Interpreter * inter);
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
        void WriteHeapLog(const string file_name);
        void WriteToLog(const char & c);
    private:
        void Reallocate(const uint64_t size);
        void Reallocate(const uint64_t size, ROObject & obj, Interpreter * inter);
        void Reallocate(const uint64_t size, uint64_t element, Interpreter * inter);
        void StartGC();
        void GCMark();
        void GCCompact();
        void GCIndexTable();
        vector<uint64_t> m_index;
        unsigned char * m_data;
        uint64_t m_capacity;
        uint64_t m_size;
        uint64_t m_ret;
        stringstream m_heap_log;
        chrono::high_resolution_clock::time_point m_start;
        int m_heap_size;
        stack<uint64_t> m_roots;
        set<reference_wrapper<uint64_t>,less<uint64_t>> m_marks;
        stack<uint64_t> m_free_index;
        
};



#endif // __HEAP_H__
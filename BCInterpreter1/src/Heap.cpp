#include "Heap.hpp"
#include "utils.hpp"
#include <variant>
#include <cstring>
#include <fstream>
#include <chrono>
#include "Interpreter.hpp"

Heap::Heap(int heap_size)
: m_heap_size(heap_size)
{
    m_start = chrono::high_resolution_clock::now();
    if (m_heap_size == 0)
    {
        m_data = new unsigned char[1000000];
        m_capacity = 1000000;
    }
    else
    {
        m_data = new unsigned char[m_heap_size * 1000000];
        m_capacity = m_heap_size * 1000000;
    }
    m_size = 0;
    m_heap_log << "timestamp,event,heap" << endl;
    WriteToLog('S');
}

Heap::~Heap()
{
    delete [] m_data;
}

void Heap::WriteHeapLog(const string file_name)
{
    ofstream file;
    file.open(file_name);
    if (!file.is_open())
        throw "Error with file " + file_name;
    file << m_heap_log.str();
    file.close();
}

void Heap::WriteToLog(const char & c)
{
    auto now = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - m_start);
    m_heap_log << now.count() << ","<< c << "," << m_size << endl;
}

void Heap::Reallocate(const uint64_t size)
{
    unsigned char * dataNew = new unsigned char[size];
    memcpy(dataNew,m_data,m_size);
    delete [] m_data;
    m_data = dataNew;
    m_capacity = size;
}

void Heap::Reallocate(const uint64_t size , ROObject & obj, Interpreter * inter)
{
    if (m_heap_size == 0)
    {
        Reallocate(size);
    }
    else
    {
        inter->GetRoots(m_roots);
        m_roots.push(obj.m_parent);
        for (auto & x : obj.m_fields)
            m_roots.push(x.second);
        StartGC();
    }

}

void Heap::Reallocate(const uint64_t size, uint64_t element, Interpreter * inter)
{
    if (m_heap_size == 0)
    {
        Reallocate(size);
    }
    else
    {
        inter->GetRoots(m_roots);
        m_roots.push(element);
        StartGC();
    }
}

uint64_t Heap::AssignLiteral(constant & obj)
{
    visit(*this,obj);
    return m_ret;
}

uint64_t Heap::AssignArray(uint32_t size, uint64_t element, Interpreter * inter) 
{
    if (m_size + (size * 8) + 5 >= m_capacity)
        Reallocate((m_size + (size * 8)) * 2,element,inter);
    if (m_size + (size * 8) + 5 >= m_capacity)
        throw "Error: Not enough space in heap.";
    
    uint64_t ret = m_size;
    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Array;
    m_size += 8;
    writeInt32_t(m_data + m_size, size);
    m_size += 4;
    for (uint32_t i = 0 ; i < size; i++)
    {
        writeInt64_t(m_data + m_size, element);
        m_size += 8;
    }
    WriteToLog('A');

    if (m_size % 2 != 0)
        m_size = m_size + 1;
    writeInt64_t(m_data + ret + 1, m_size - ret);

    if (m_free_index.empty())
    {
        m_index.push_back(ret);
        return (m_index.size() - 1)<<2;
    }
    else
    {
        m_index[m_free_index.top()] = ret;
        uint64_t index = m_free_index.top();
        m_free_index.pop();
        return index<<2;
    }
}

uint64_t Heap::AssignObject(ROObject & obj, Interpreter * inter) 
{
    if (m_size + obj.m_size + 21 >= m_capacity)
        Reallocate((m_size + (obj.m_size + 21)) * 2,obj,inter);
    if (m_size + obj.m_size + 21 >= m_capacity)
        throw "Error: Not enough space in heap.";
    
    uint64_t ret = m_size;
    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Object;
    m_size += 8;
    writeInt64_t(m_data + m_size, obj.m_parent);
    m_size += 8;
    uint64_t pos_for_length = m_size;
    m_size += 8;
    writeInt16_t(m_data + m_size, obj.m_fields.size());
    m_size += 2;
    for (auto & it : obj.m_fields)
    {
        uint8_t size = it.first.size();
        writeInt8_t(m_data + m_size,size);
        m_size++;
        writeString(m_data + m_size,it.first);
        m_size += size;
        writeInt64_t(m_data + m_size,it.second);
        m_size += 8;
    }
    writeInt64_t(m_data + pos_for_length, m_size - pos_for_length);
    writeInt16_t(m_data + m_size, obj.m_methods.size());
    m_size += 2;
    for (auto & it : obj.m_methods)
    {
        uint8_t size = it.first.size();
        writeInt8_t(m_data + m_size,size);
        m_size++;
        writeString(m_data + m_size,it.first);
        m_size += size;
        writeInt16_t(m_data + m_size,it.second);
        m_size += 2;
    }
    WriteToLog('A');

    if (m_size % 2 != 0)
        m_size = m_size + 1;
    writeInt64_t(m_data + ret + 1, m_size - ret);

    if (m_free_index.empty())
    {
        m_index.push_back(ret);
        return (m_index.size() - 1)<<2;
    }
    else
    {
        m_index[m_free_index.top()] = ret;
        uint64_t index = m_free_index.top();
        m_free_index.pop();
        return index<<2;
    }
}

runObj Heap::GetRunObject(uint64_t index)
{
    RuntimeObject::Opcode type;
    uint64_t HeapIndex;
    if (index % 4 == 1)
        type = RuntimeObject::Opcode::Integer;
    else if (index % 4 == 2)
        type = RuntimeObject::Opcode::Boolean;
    else if (index % 4 == 3)
        type = RuntimeObject::Opcode::Null;
    else
    {
        index = index>>2;
        HeapIndex = m_index[index];
        type = static_cast<RuntimeObject::Opcode>(readInt8_t(m_data,HeapIndex));
        HeapIndex++;
    }
    switch (type){
        case RuntimeObject::Opcode::Integer:
        {
            ROInteger num;
            num.m_value = index>>2;
            return runObj(num);
        }
        case RuntimeObject::Opcode::Boolean:
        {
            ROBoolean b;
            b.m_value = index>>2;
            return runObj(b);
        }
        case RuntimeObject::Opcode::Null:
        {
            return runObj(RONull{});
        }
        case RuntimeObject::Opcode::Array:
        {
            return runObj(ROArray{});
        }
        case RuntimeObject::Opcode::Object:
        {
            ROObject obj;
            HeapIndex += 8;
            obj.m_parent = readInt64_t(m_data,HeapIndex);
            return runObj(obj);
        }
        default:
            break;
    }
    throw "GetRunObjectException.";
}

uint64_t Heap::GetSetField(uint64_t index, const string & name, uint64_t value, bool get) 
{
    index = index>>2;

    uint64_t HeapIndex = m_index[index];
    RuntimeObject::Opcode type = static_cast<RuntimeObject::Opcode>(readInt8_t(m_data,HeapIndex));
    HeapIndex++;
    switch (type){
        case RuntimeObject::Opcode::Object:
        {
            HeapIndex += 24;
            uint16_t size = readInt16_t(m_data,HeapIndex);
            HeapIndex += 2;
            for (uint16_t i = 0; i < size; i++)
            {
                uint8_t size_str = readInt8_t(m_data,HeapIndex);
                HeapIndex++;
                string str;
                readString(m_data,HeapIndex,size_str,str);
                HeapIndex += size_str;
                if (str == name)
                    if (get)
                        return readInt64_t(m_data,HeapIndex);
                    else{
                        writeInt64_t(m_data+HeapIndex,value);
                        return 0;
                    }
                else
                    HeapIndex += 8;
            }
            break;
        }
        default:
            throw "Error: Only objects has fields";
    }
    throw "Error: Cannot find given field.";
}

uint64_t Heap::ArrayGet(uint64_t index, uint32_t pos) 
{
    index = index>>2;
    uint64_t HeapIndex = m_index[index];
    HeapIndex += 13;
    HeapIndex += pos * 8;
    return readInt64_t(m_data,HeapIndex);
}

void Heap::ArraySet(uint64_t index, uint32_t pos, uint64_t value) 
{
    index = index>>2;
    uint64_t HeapIndex = m_index[index];
    HeapIndex += 13;
    HeapIndex += pos * 8;
    writeInt64_t(m_data + HeapIndex,value);
}

uint16_t Heap::GetMethod(uint64_t index,const string & name) 
{
    index = index>>2;
    uint64_t HeapIndex = m_index[index];
    HeapIndex += 17;
    uint64_t start = readInt64_t(m_data,HeapIndex);
    HeapIndex += start;
    uint16_t size = readInt16_t(m_data,HeapIndex);
    HeapIndex += 2;
    for (uint16_t i = 0; i < size; i++)
    {
        uint8_t size_str = readInt8_t(m_data,HeapIndex);
        HeapIndex++;
        string str;
        readString(m_data,HeapIndex,size_str,str);
        HeapIndex += size_str;
        if (str == name)
            return readInt16_t(m_data,HeapIndex);
        else
            HeapIndex += 2;
    }
    return 0;
}

void Heap::operator ()(OInteger & integer)
{
    m_ret = integer.m_value;
    m_ret = m_ret<<2;
    m_ret += 1;
}

void Heap::operator ()(OBoolean & boolean)
{
    m_ret = boolean.m_value;
    m_ret = m_ret<<2;
    m_ret += 2;
}

void Heap::operator ()(ONull & nul)
{
    m_ret = 3;
}

template<typename A>
void Heap::operator ()(A & obj)
{
    throw "Wrong type for heap!";
}


const string Heap::GetAsString(uint64_t index) 
{
    RuntimeObject::Opcode type;
    uint64_t HeapIndex;
    if (index % 4 == 1)
        type = RuntimeObject::Opcode::Integer;
    else if (index % 4 == 2)
        type = RuntimeObject::Opcode::Boolean;
    else if (index % 4 == 3)
        type = RuntimeObject::Opcode::Null;
    else
    {
        index = index>>2;
        HeapIndex = m_index[index];
        type = static_cast<RuntimeObject::Opcode>(readInt8_t(m_data,HeapIndex));
        HeapIndex++;
    }
    switch (type){
        case RuntimeObject::Opcode::Integer:
        {
            return to_string((int32_t)(index>>2));
        }
        case RuntimeObject::Opcode::Boolean:
        {
            if (index>>2 == 0)
                return "false";
            else
                return "true";
        }
        case RuntimeObject::Opcode::Null:
        {
            return "null";
        }
        case RuntimeObject::Opcode::Array:
        {
            string str;
            str = "[";
            HeapIndex += 8;
            uint32_t size = readInt32_t(m_data,HeapIndex);
            HeapIndex += 4;
            for (uint32_t i = 0; i < size; i++)
            {
                str += GetAsString(readInt64_t(m_data,HeapIndex + (i * 8)));
                if (i < size - 1)
                    str += ", ";
            }
            str += "]";
            return str;
        }
        case RuntimeObject::Opcode::Object:
        {
            string str = "object(";
            HeapIndex += 8;
            uint64_t parent = readInt64_t(m_data,HeapIndex);
            if (parent != 3)
            {
                str += "..=";
                str += GetAsString(parent);
            }
            HeapIndex += 16;
            uint16_t size = readInt16_t(m_data,HeapIndex);
            HeapIndex += 2;
            for (uint16_t i = 0; i < size; i++)
            {
                uint8_t name_size = readInt8_t(m_data,HeapIndex);
                HeapIndex++;
                string name;
                readString(m_data,HeapIndex,name_size,name);
                str += name + "=";
                HeapIndex += name_size;
                str += GetAsString(readInt64_t(m_data,HeapIndex));
                HeapIndex += 8;
                if (i < size - 1)
                    str += ", ";
            }
            str += ")";
            return str;
        }
        default:
            break;
    }
    throw "Error : Something wrong with print argument.";
}

void Heap::StartGC()
{
    GCMark();
    GCCompact();
    m_marks.clear();
    GCIndexTable();

    WriteToLog('G');
}

void Heap::GCMark()
{
    while (!m_roots.empty())
    {
        uint64_t index = m_roots.top();
        m_roots.pop();
        if (index % 4 != 0)
            continue;
        index = index>>2;
        uint64_t heap = m_index[index];
        if (heap % 2 == 1)
            continue;
        m_index[index]++;
        m_marks.insert(m_index[index]);
        RuntimeObject::Opcode type;
        type = static_cast<RuntimeObject::Opcode>(readInt8_t(m_data,heap));

        switch (type){
            case RuntimeObject::Opcode::Array:
            {
                heap++;
                heap += 8;
                uint32_t size = readInt32_t(m_data,heap);
                heap += 4;
                for (uint32_t i = 0; i < size; i++)
                {
                    uint64_t el = readInt64_t(m_data,heap);
                    if (el % 4 == 0)
                        m_roots.push(el);
                    heap += 8;
                }
                break;
            }
            case RuntimeObject::Opcode::Object:
            {
                heap++;
                heap += 8;
                m_roots.push(readInt64_t(m_data,heap));
                heap += 16;
                uint16_t size = readInt16_t(m_data,heap);
                heap += 2;
                for (uint32_t i = 0; i < size; i++)
                {
                    uint8_t size_str = readInt8_t(m_data,heap);
                    heap += 1 + size_str;
                    uint64_t el = readInt64_t(m_data,heap);
                    if (el % 4 == 0)
                        m_roots.push(el);
                    heap += 8;
                }
                break;
            }
            default:
                break;
        }     
    }
}

void Heap::GCCompact()
{
    uint64_t current = 0;
    for (auto & x : m_marks)
    {
        uint64_t now = x.get();
        uint64_t size = readInt64_t(m_data,now);
        if (now - 1 != current){
            memmove(m_data + current,m_data + now - 1,size);
            x.get() = current + 1;
        }
        current += size;
    }
    m_size = current;
}

void Heap::GCIndexTable()
{
    m_free_index = stack<uint64_t>();
    while (m_index.back() % 2 == 0)
        m_index.pop_back();
    for (int64_t i = m_index.size() - 1; i >= 0; i--)
    {
        if (m_index[i] % 2 == 1)
            m_index[i]--;
        else
        {
            m_index[i] = 0;
            m_free_index.push(i);
        }
    }
}
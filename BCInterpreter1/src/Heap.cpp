#include "Heap.hpp"
#include "utils.hpp"
#include <variant>
#include <cstring>

Heap::Heap()
{
    m_data = new unsigned char[100000];
    m_capacity = 100000;
    m_size = 0;
}

Heap::~Heap()
{
    delete [] m_data;
}

void Heap::Reallocate(const uint64_t size)
{
    unsigned char * dataNew = new unsigned char[size];
    memcpy(dataNew,m_data,m_size);
    delete [] m_data;
    m_data = dataNew;
    m_capacity = size;

}

uint64_t Heap::AssignLiteral(constant & obj)
{
//    uint64_t ret = m_size;
    visit(*this,obj);
    return m_ret;
//    if (m_size == ret)
//        return 0;
//    m_index.push_back(ret);
//    return m_index.size() - 1;
}

uint64_t Heap::AssignArray(uint32_t size, uint64_t element) 
{
    if (m_size + (size * 4) + 5 >= m_capacity)
        Reallocate(m_size + (size * 4) * 2);
    uint64_t ret = m_size;
    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Array;
    writeInt32_t(m_data + m_size, size);
    m_size += 4;
    for (uint32_t i = 0 ; i < size; i++)
    {
        writeInt64_t(m_data + m_size, element);
        m_size += 8;
    }
    if (m_size % 4 != 0)
        m_size = m_size + 4 - (m_size % 4);
    m_index.push_back(ret);
    return (m_index.size() - 1)<<2;
}

uint64_t Heap::AssignObject(ROObject & obj) 
{
    if (m_size + 9 >= m_capacity)
        Reallocate(m_size * 2);
    uint64_t ret = m_size;
    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Object;
    writeInt64_t(m_data + m_size, obj.m_parent);
    m_size += 8;
    uint64_t pos_for_length = m_size;
    m_size += 8;
    writeInt16_t(m_data + m_size, obj.m_fields.size());
    m_size += 2;
    for (auto & it : obj.m_fields)
    {
        uint8_t size = it.first.size();
        if (m_size + size + 5 >= m_capacity)
            Reallocate((m_size + size) * 2);
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
        if (m_size + size + 3 >= m_capacity)
            Reallocate((m_size + size) * 2);
        writeInt8_t(m_data + m_size,size);
        m_size++;
        writeString(m_data + m_size,it.first);
        m_size += size;
        writeInt16_t(m_data + m_size,it.second);
        m_size += 2;
    }
    if (m_size % 4 != 0)
        m_size = m_size + 4 - (m_size % 4);
    m_index.push_back(ret);
    return (m_index.size() - 1)<<2;
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
            HeapIndex += 16;
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
    HeapIndex += 5;
    HeapIndex += pos * 8;
    return readInt64_t(m_data,HeapIndex);
}

void Heap::ArraySet(uint64_t index, uint32_t pos, uint64_t value) 
{
    index = index>>2;
    uint64_t HeapIndex = m_index[index];
    HeapIndex += 5;
    HeapIndex += pos * 8;
    writeInt64_t(m_data + HeapIndex,value);
}

uint16_t Heap::GetMethod(uint64_t index,const string & name) 
{
    index = index>>2;
    uint64_t HeapIndex = m_index[index];
    HeapIndex += 9;
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

//    if (m_size + 5 >= m_capacity)
//        Reallocate(m_size * 2);
//    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Integer;
//    writeInt32_t(m_data + m_size, integer.m_value);
//    m_size += 4;
}

void Heap::operator ()(OBoolean & boolean)
{
    m_ret = boolean.m_value;
    m_ret = m_ret<<2;
    m_ret += 2;

//    if (m_size + 2 >= m_capacity)
//        Reallocate(m_size * 2);
//    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Boolean;
//    writeInt8_t(m_data + m_size, boolean.m_value);
//    m_size++;
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
            //return to_string((int32_t)readInt32_t(m_data,HeapIndex));
        }
        case RuntimeObject::Opcode::Boolean:
        {
            if (index>>2 == 0)
                return "false";
            else
                return "true";
            //if (readInt8_t(m_data,HeapIndex) == 0)
            //    return "false";
            //else
            //    return "true";
        }
        case RuntimeObject::Opcode::Null:
        {
            return "null";
        }
        case RuntimeObject::Opcode::Array:
        {
            string str;
            str = "[";
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
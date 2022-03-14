#include "Heap.hpp"
#include "utils.hpp"

Heap::Heap()
{
    m_data = new unsigned char[1000];
    m_data[0] = (unsigned char)RuntimeObject::Opcode::Null;
    m_capacity = 1000;
    m_size = 1;
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
}

uint64_t Heap::AssignLiteral(constant & obj)
{
    uint64_t ret = m_size;
    visit(*this,obj);
    if (m_size == ret)
        return 0;
    return ret;
}

runObj Heap::GetRunObject(uint32_t index)
{
    RuntimeObject::Opcode type = static_cast<RuntimeObject::Opcode>(readInt8_t(m_data,index));
    index++;
    switch (type){
        case RuntimeObject::Opcode::Integer:
        {
            ROInteger num;
            num.m_value = readInt32_t(m_data,index);
            return runObj(num);
        }
        case RuntimeObject::Opcode::Boolean:
        {
            ROBoolean b;
            b.m_value = readInt8_t(m_data,index);
            return runObj(b);
        }
        case RuntimeObject::Opcode::Null:
        {
            return runObj(RONull{});
        }
        default:
            break;
    }
    throw "GetRunObjectException.";
}

void Heap::operator ()(OInteger & integer)
{
    if (m_size + 5 >= m_capacity)
        Reallocate(m_size * 2);
    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Integer;
    writeInt32_t(m_data + m_size, integer.m_value);
    m_size += 4;
}

void Heap::operator ()(OBoolean & boolean)
{
    if (m_size + 2 >= m_capacity)
        Reallocate(m_size * 2);
    m_data[m_size++] = (unsigned char)RuntimeObject::Opcode::Boolean;
    writeInt8_t(m_data + m_size, boolean.m_value);
    m_size++;
}

void Heap::operator ()(ONull & nul)
{
}

template<typename A>
void Heap::operator ()(A & obj)
{
    throw "Wrong type for heap!";
}

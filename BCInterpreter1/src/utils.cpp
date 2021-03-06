#include "utils.hpp"

uint8_t readInt8_t(unsigned char * code, uint64_t index)
{
    return code[index];
}

uint16_t readInt16_t(unsigned char * code, uint64_t index)
{
    uint16_t value = 0;
    for (int i = 1 ; i >= 0; i--)
    {
        value <<= 8;
        value |= code[index + i];
    }
    return value;
}

uint32_t readInt32_t(unsigned char * code, uint64_t index)
{
    uint32_t value = 0;
    for (int i = 3 ; i >= 0; i--)
    {
        value <<= 8;
        value |= code[index + i];
    }
    return value;
}

uint64_t readInt64_t(unsigned char * code, uint64_t index)
{
    uint64_t value = 0;
    for (int i = 7; i >= 0; i--)
    {
        value <<= 8;
        value |= code[index + i];
    }
    return value;
}

void readString(unsigned char * code, uint64_t index, uint32_t size, string & res)
{
    res.append((char*)code + index,size);
}

void writeInt8_t(unsigned char * code,uint8_t value)
{
    code[0] = value;
}

void writeInt16_t(unsigned char * code,uint16_t value)
{
    code[0] = value & 0xFF;
    code[1] = (value >> 8) & 0xFF;
}

void writeInt32_t(unsigned char * code,uint32_t value)
{
    code[0] = value & 0xFF;
    code[1] = (value >> 8) & 0xFF;
    code[2] = (value >> 16) & 0xFF;
    code[3] = (value >> 24) & 0xFF;    
}

void writeInt64_t(unsigned char * code,uint64_t value)
{
    code[0] = value & 0xFF;
    code[1] = (value >> 8) & 0xFF;
    code[2] = (value >> 16) & 0xFF;
    code[3] = (value >> 24) & 0xFF;
    code[4] = (value >> 32) & 0xFF;
    code[5] = (value >> 40) & 0xFF;
    code[6] = (value >> 48) & 0xFF;
    code[7] = (value >> 56) & 0xFF;
}

void writeString(unsigned char * code,const string & name)
{
    name.copy((char*)code,name.size());
}

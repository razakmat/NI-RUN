#include "ProgramObject.hpp"
#include "../Utils/utils.hpp"

OInteger::OInteger(int32_t v)
:m_value(v)
{
    
}

void OInteger::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Integer);
    writeInt32_t(code+1,m_value);
    size = 5;
}

OBoolean::OBoolean(bool v)
:m_value(v)
{
    
}

void OBoolean::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Boolean);
    writeInt8_t(code+1,m_value);
    size = 2;
}

void ONull::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Null);
    size = 1;
}

void OString::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::String);
    writeInt32_t(code+1,m_length);
    writeString(code+5,m_characters);
    size = 5 + m_length;
}

void OSlot::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Slot);
    writeInt16_t(code+1,m_name);
    size = 3;
}

void OMethod::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Method);
    writeInt16_t(code+1,m_name);
    writeInt8_t(code+3,m_arguments);
    writeInt16_t(code+4,m_locals);
    writeInt32_t(code+6,m_length);
    size = 10;
    uint16_t sizeI;
    for (uint32_t i = 0; i < m_length; i++)
    {
        visit([&](auto && v){v.writeByteCode(code + size,sizeI);},(*m_ins)[i]);
        size += sizeI;
    }
}

void OClass::writeByteCode(unsigned char * code, uint32_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Class);
    writeInt16_t(code+1,m_length);
    for (uint16_t i = 0; i < m_length; i++)
        writeInt16_t(code+3+(i*2),m_members[i]);
    size = 3 + (m_length * 2);
}

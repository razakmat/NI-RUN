#ifndef __PROGRAMOBJECT_H__
#define __PROGRAMOBJECT_H__

#include <cstdint>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include "../Structures/Instruction.hpp"

using namespace std;

struct ProgramObject
{
    enum class Opcode
    {
        Integer = 0x00,
        Null = 0x01,
        String = 0x02,
        Method = 0x03,
        Slot = 0x04,
        Class = 0x05,
        Boolean = 0x06
    };
    virtual void writeByteCode(unsigned char * code, uint32_t & size) = 0;
};

struct OInteger : public ProgramObject 
{
    OInteger() = default;
    OInteger(int32_t v);
    void writeByteCode(unsigned char * code, uint32_t & size);
    int32_t m_value;
};

struct OBoolean : public ProgramObject 
{
    OBoolean() = default;
    OBoolean(bool v);
    void writeByteCode(unsigned char * code, uint32_t & size);
    bool m_value;
};

struct ONull : public ProgramObject 
{
    void writeByteCode(unsigned char * code, uint32_t & size);
};

struct OString : public ProgramObject 
{
    void writeByteCode(unsigned char * code, uint32_t & size);
    uint32_t m_length;
    string m_characters;
};

struct OSlot : public ProgramObject 
{
    void writeByteCode(unsigned char * code, uint32_t & size);
    uint16_t m_name;
};

struct OMethod : public ProgramObject 
{
    void writeByteCode(unsigned char * code, uint32_t & size);
    uint32_t m_length;
    uint32_t m_start = 0;
    uint16_t m_name;
    uint16_t m_locals;
    uint8_t m_arguments;
    shared_ptr<vector<ins>> m_ins;
};

struct OClass : public ProgramObject 
{
    void writeByteCode(unsigned char * code, uint32_t & size);
    uint16_t m_length;
    vector<uint16_t> m_members;
};

using constant = variant<OInteger,OBoolean,ONull,OString,OSlot,OMethod,OClass>;


#endif // __PROGRAMOBJECT_H__
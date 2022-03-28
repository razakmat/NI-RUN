#ifndef __PROGRAMOBJECT_H__
#define __PROGRAMOBJECT_H__

#include <cstdint>
#include <string>
#include <vector>
#include <variant>

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
};

struct OInteger : public ProgramObject 
{
    OInteger() = default;
    OInteger(int32_t v);
    int32_t m_value;
};

struct OBoolean : public ProgramObject 
{
    OBoolean() = default;
    OBoolean(bool v);
    bool m_value;
};

struct ONull : public ProgramObject 
{
};

struct OString : public ProgramObject 
{
    uint32_t m_length;
    string m_characters;
};

struct OSlot : public ProgramObject 
{
    uint16_t m_name;
};

struct OMethod : public ProgramObject 
{
    uint32_t m_length;
    uint32_t m_start;
    uint16_t m_name;
    uint16_t m_locals;
    uint8_t m_arguments;


};

struct OClass : public ProgramObject 
{
    uint16_t m_length;
    vector<uint16_t> m_members;
};

using constant = variant<OInteger,OBoolean,ONull,OString,OSlot,OMethod,OClass>;


#endif // __PROGRAMOBJECT_H__
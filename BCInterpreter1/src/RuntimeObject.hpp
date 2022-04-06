#ifndef __RUNTIMEOBJECT_H__
#define __RUNTIMEOBJECT_H__

#include <cstdint>
#include <variant>
#include <vector>
#include <map>
#include <string>

using namespace std;

struct RuntimeObject
{
    enum class Opcode
    {
        Integer = 0x00,
        Boolean = 0x01,
        Null = 0x02,
        Array = 0x03,
        Object = 0x04,
    };
};

struct ROInteger : public RuntimeObject 
{
    int32_t m_value;
};

struct ROBoolean : public RuntimeObject 
{
    bool m_value;
};

struct RONull : public RuntimeObject 
{
};

struct ROArray : public RuntimeObject
{
    uint32_t m_length;
    vector<uint32_t> m_pointers;
};

struct ROObject : public RuntimeObject
{
    uint32_t m_parent;
    map<string,uint32_t> m_fields;
    map<string,uint16_t> m_methods;
};

using runObj = variant<ROInteger,ROBoolean,RONull,ROArray,ROObject>;


#endif // __RUNTIMEOBJECT_H__

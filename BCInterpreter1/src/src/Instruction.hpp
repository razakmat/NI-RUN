#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <cstdint>
#include <variant>

using namespace std;

struct Instruction
{
    enum class Opcode
    {
        Label = 0x00,
        Literal = 0x01,
        Print = 0x02,
        Array = 0x03,
        Object = 0x04,
        Get_Field = 0x05,
        Set_Field = 0x06,
        Call_Method = 0x07,
        Call_Function = 0x08,
        Set_Local = 0x09,
        Get_Local = 0x0A,
        Set_Global = 0x0B,
        Get_Global = 0x0C,
        Branch = 0x0D,
        Jump = 0x0E,
        Return = 0x0F,
        Drop = 0x10
    };
    uint16_t m_index;
};

struct ILiteral : public Instruction
{
};

struct IGet_Local : public Instruction
{
};

struct ISet_Local : public Instruction
{
};

struct IGet_Global : public Instruction
{
};

struct ISet_Global : public Instruction
{
};

struct ICall_Function : public Instruction
{
    uint8_t m_arguments;
};

struct IReturn : public Instruction
{
};

struct ILabel : public Instruction
{
};

struct IJump : public Instruction
{
};

struct IBranch : public Instruction
{
};

struct IPrint : public Instruction
{
    uint8_t m_arguments;
};

struct IArray : public Instruction
{
};

struct IObject : public Instruction
{
};

struct IGet_Field : public Instruction
{
};

struct ISet_Field : public Instruction
{
};

struct ICall_Method : public Instruction
{
    uint8_t m_arguments;
};

struct IDrop : public Instruction
{
};

using ins = variant<ILiteral,IGet_Local,ISet_Local,IGet_Global,
                    ISet_Global,ICall_Function,IReturn,ILabel,
                    IJump,IBranch,IPrint,IArray,IObject,IGet_Field,
                    ISet_Field,ICall_Method,IDrop>;


#endif // __INSTRUCTION_H__
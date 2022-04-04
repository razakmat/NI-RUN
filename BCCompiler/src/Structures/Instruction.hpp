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
    virtual void writeByteCode(unsigned char * code, uint16_t & size) = 0;
    uint16_t m_index;
};

struct ILiteral : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IGet_Local : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct ISet_Local : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IGet_Global : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct ISet_Global : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct ICall_Function : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
    uint8_t m_arguments;
};

struct IReturn : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct ILabel : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IJump : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IBranch : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IPrint : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
    uint8_t m_arguments;
};

struct IArray : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IObject : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct IGet_Field : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct ISet_Field : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

struct ICall_Method : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
    uint8_t m_arguments;
};

struct IDrop : public Instruction
{
    void writeByteCode(unsigned char * code, uint16_t & size);
};

using ins = variant<ILiteral,IGet_Local,ISet_Local,IGet_Global,
                    ISet_Global,ICall_Function,IReturn,ILabel,
                    IJump,IBranch,IPrint,IArray,IObject,IGet_Field,
                    ISet_Field,ICall_Method,IDrop>;


#endif // __INSTRUCTION_H__
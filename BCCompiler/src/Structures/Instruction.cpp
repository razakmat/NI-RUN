#include "Instruction.hpp"
#include "../Utils/utils.hpp"

void ILiteral::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Literal);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void IGet_Local::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Get_Local);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void ISet_Local::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Set_Local);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void IGet_Global::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Get_Global);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void ISet_Global::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Set_Global);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void ICall_Function::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Call_Function);
    writeInt16_t(code+1,m_index);
    writeInt8_t(code+3,m_arguments);
    size = 4;
}

void IReturn::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Return);
    size = 1;
}

void ILabel::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Label);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void IJump::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Jump);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void IBranch::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Branch);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void IPrint::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Print);
    writeInt16_t(code+1,m_index);
    writeInt8_t(code+3,m_arguments);
    size = 4;
}

void IArray::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Array);
    size = 1;
}

void IObject::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Object);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void IGet_Field::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Get_Field);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void ISet_Field::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Set_Field);
    writeInt16_t(code+1,m_index);
    size = 3;
}

void ICall_Method::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Call_Method);
    writeInt16_t(code+1,m_index);
    writeInt8_t(code+3,m_arguments);
    size = 4;
}

void IDrop::writeByteCode(unsigned char * code, uint16_t & size) 
{
    writeInt8_t(code,(uint8_t)Opcode::Drop);
    size = 1;
}

#include "Parser.hpp"

#include "ProgramObject.hpp"
#include "Instruction.hpp"
#include "utils.hpp"

#include <unordered_map>
#include <string>
#include <iostream>

Parser::~Parser()
{
    if (m_code != nullptr)
        delete m_code;
}

void Parser::LoadFile(const string & name)
{
    streampos size = 0;
    ifstream file(name,ios::binary);

    if (!file.is_open())
    {
        throw "ERROR : Wrong file name!";
    }

    size = file.tellg();
    file.seekg(0,ios::end);
    size = file.tellg() - size;
    file.seekg(0,ios::beg);

    m_pos = 0;
    m_size = size;
    m_code = new unsigned char[m_size];
    file.read((char*)m_code,m_size);
}

void Parser::ReadInstruction(FMLVM * memory)
{
    Instruction::Opcode type = static_cast<Instruction::Opcode>(readInt8_t(m_code,m_pos));
    m_pos++;
    switch (type){
        case Instruction::Opcode::Literal:
        {
            ILiteral ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Get_Local:
        {
            IGet_Local ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Set_Local:
        {
            ISet_Local ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Get_Global:
        {
            IGet_Global ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Set_Global:
        {
            ISet_Global ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Call_Function:
        {
            ICall_Function ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            ins.m_arguments = readInt8_t(m_code,m_pos);
            m_pos ++;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Return:
        {
            memory->m_instructions.push_back(IReturn{});
            break;
        }
        case Instruction::Opcode::Label:
        {
            ILabel ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            OString str = get<OString>(memory->m_constant_pool[ins.m_index]);
            memory->m_labels.insert({str.m_characters,memory->m_instructions.size()+1});
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Jump:
        {
            IJump ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);   
            break;
        }
        case Instruction::Opcode::Branch:
        {
            IBranch ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Print:
        {
            IPrint ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            ins.m_arguments = readInt8_t(m_code,m_pos);
            m_pos++;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Array:
        {
            memory->m_instructions.push_back(IArray{});
            break;
        }
        case Instruction::Opcode::Object:
        {
            IObject ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Get_Field:
        {
            IGet_Field ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Set_Field:
        {
            ISet_Field ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Call_Method:
        {
            ICall_Method ins;
            ins.m_index = readInt16_t(m_code,m_pos);
            m_pos += 2;
            ins.m_arguments = readInt8_t(m_code,m_pos);
            m_pos++;
            memory->m_instructions.push_back(ins);
            break;
        }
        case Instruction::Opcode::Drop:
        {
            memory->m_instructions.push_back(IDrop{});
            break;
        }
    }
}

void Parser::ReadGlobals(FMLVM * memory)
{
    uint16_t size = readInt16_t(m_code,m_pos);
    m_pos += 2;
    for (uint16_t i = 0; i < size; i++)
    {
        uint16_t index = readInt16_t(m_code,m_pos);
        if (holds_alternative<OSlot>(memory->m_constant_pool[index]))
        {
            OString & str = get<OString>(memory->m_constant_pool[
                get<OSlot>(memory->m_constant_pool[index]).m_name]);
            memory->m_frame_stack.InsertGlobal(str.m_characters,0);
        }
        else if (holds_alternative<OMethod>(memory->m_constant_pool[index]))
        {
            OString & str = get<OString>(memory->m_constant_pool[
                get<OMethod>(memory->m_constant_pool[index]).m_name]);
            memory->m_frame_stack.InsertGlobal(str.m_characters,index);
        }
        else
            throw "Error: Globals can contain only methods and slots.";
        m_pos += 2;
    }
}

void Parser::ReadEntryPoint(FMLVM * memory)
{
    memory->m_entry_point = readInt16_t(m_code,m_pos);
    m_pos += 2;
    if (m_pos != m_size)
        throw "ERROR : After parsing file still has unread bytes of code!";
}

void Parser::ReadConstantPool(FMLVM * memory)
{
    uint16_t size = readInt16_t(m_code,m_pos);
    m_pos += 2;
    for (uint16_t i = 0; i < size; i++)
    {
        ProgramObject::Opcode type = static_cast<ProgramObject::Opcode>(readInt8_t(m_code,m_pos));
        m_pos++;
        
        switch (type){
            case ProgramObject::Opcode::Boolean:
            {
                OBoolean obj;
                obj.m_value = readInt8_t(m_code,m_pos);
                m_pos++;
                memory->m_constant_pool.push_back(obj);
                break;
            }
            case ProgramObject::Opcode::Integer:
            {
                OInteger obj;
                obj.m_value = readInt32_t(m_code,m_pos);
                m_pos += 4;
                memory->m_constant_pool.push_back(obj);
                break;
            }
            case ProgramObject::Opcode::Null:
            {
                memory->m_constant_pool.push_back(ONull{});
                break;
            }
            case ProgramObject::Opcode::String:
            {
                OString obj;
                obj.m_length = readInt32_t(m_code,m_pos);
                m_pos += 4;
                readString(m_code,m_pos,obj.m_length,obj.m_characters);
                m_pos += obj.m_length;
                memory->m_constant_pool.push_back(obj);
                break;
            }
            case ProgramObject::Opcode::Slot:
            {
                OSlot obj;
                obj.m_name = readInt16_t(m_code,m_pos);
                m_pos += 2;
                memory->m_constant_pool.push_back(obj);
                break;
            }
            case ProgramObject::Opcode::Class:
            {
                OClass obj;
                obj.m_length = readInt16_t(m_code,m_pos);
                m_pos += 2;
                for (uint16_t i = 0; i < obj.m_length; i++)
                {
                    obj.m_members.push_back(readInt16_t(m_code,m_pos));
                    m_pos +=2;
                }
                memory->m_constant_pool.push_back(obj);
                break;
            }
            case ProgramObject::Opcode::Method:
            {
                OMethod obj;
                obj.m_name = readInt16_t(m_code,m_pos);
                m_pos += 2;
                obj.m_arguments = readInt8_t(m_code,m_pos);
                m_pos++;
                obj.m_locals = readInt16_t(m_code,m_pos);
                m_pos += 2;
                obj.m_length = readInt32_t(m_code,m_pos);
                m_pos += 4;
                obj.m_start = (uint32_t)memory->m_instructions.size();
                for (uint32_t i = 0; i < obj.m_length; i++)
                    ReadInstruction(memory);
                memory->m_constant_pool.push_back(obj);
                break;
            }

        }
    }
}

void Parser::ParseCode(FMLVM * memory)
{
    ReadConstantPool(memory);
    ReadGlobals(memory);
    ReadEntryPoint(memory);
}



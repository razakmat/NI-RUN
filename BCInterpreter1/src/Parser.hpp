#ifndef __PARSER_H__
#define __PARSER_H__

#include <vector>
#include <fstream>
#include <string>
#include "FMLVM.hpp"


using namespace std;

class Parser
{
    public:
        ~Parser();
        void LoadFile(const string & name);
        void ParseCode(FMLVM * memory);
    private:
        void ReadConstantPool(FMLVM * memory);
        void ReadInstruction(FMLVM * memory);
        void ReadGlobals(FMLVM * memory);
        void ReadEntryPoint(FMLVM * memory);
        unsigned char * m_code = nullptr;
        uint32_t m_size;
        uint32_t m_pos;
};


#endif // __PARSER_H__
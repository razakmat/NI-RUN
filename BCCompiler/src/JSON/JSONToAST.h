#ifndef JSONTOAST_H
#define JSONTOAST_H

#include <fstream>
#include "./nlohmann/json.hpp"
#include "../Structures/AST.h"

using namespace std;
using json = nlohmann::json;

struct ParserException : public exception
{
    const char* what () const throw()
    {
        return "Error parsing JSON";
    }
};

class JSONToAST
{
    public:
        AST * convert(ifstream & file);
        AST * convertFun(const string & s);
    private:
        AST * convertSwitch(json & j);
        ASTInteger * JSONInteger(json & j);
        ASTBoolean * JSONBoolean(json & j);
        ASTVariable * JSONVariable(json & j);
        ASTAccessVariable * JSONAccessVariable(json & j);
        ASTAssignVariable * JSONAssignVariable(json & j);
        ASTArray * JSONArray(json & j);
        ASTAccessArray * JSONAccessArray(json & j);
        ASTAssignArray * JSONAssignArray(json & j);
        ASTFunction * JSONFunction(json & j);
        ASTCallFunction * JSONCallFunction(json & j);
        ASTPrint * JSONPrint(json & j);
        ASTBlock * JSONBlock(json & j);
        ASTTop * JSONTop(json & j);
        ASTLoop * JSONLoop(json & j);
        ASTConditional * JSONConditional(json & j);
        ASTObject * JSONObject(json & j);
        ASTAssignField * JSONAssignField(json & j);
        ASTAccessField * JSONAccessField(json & j);
        ASTCallMethod * JSONCallMethod(json & j);
};


#endif
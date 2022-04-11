#include "JSONToAST.h"
#include "./nlohmann/json.hpp"

#include <vector>

using namespace std;
using json = nlohmann::json;


AST * JSONToAST::convert(ifstream & file)
{
    try{
        json j = json::parse(file);
        return convertSwitch(j);
    }catch(...){
        throw ParserException();
    }
}

AST * JSONToAST::convertFun(const string & s)
{
    json j = json::parse(s);
    return convertSwitch(j);
}


AST * JSONToAST::convertSwitch(json & j)
{

    if (j.begin().value() == "Null")
        return new ASTNull();
    if (j.begin().key() == "Top")
        return JSONTop(j.begin().value());
    if (j.begin().key() == "Integer")
        return JSONInteger(j.begin().value());
    if (j.begin().key() == "Boolean")
        return JSONBoolean(j.begin().value());
    if (j.begin().key() == "Variable")
        return JSONVariable(j.begin().value());
    if (j.begin().key() == "AccessVariable")
        return JSONAccessVariable(j.begin().value());
    if (j.begin().key() == "AssignVariable")
        return JSONAssignVariable(j.begin().value());
    if (j.begin().key() == "Array")
        return JSONArray(j.begin().value());
    if (j.begin().key() == "AccessArray")
        return JSONAccessArray(j.begin().value());
    if (j.begin().key() == "AssignArray")
        return JSONAssignArray(j.begin().value());
    if (j.begin().key() == "Function")
        return JSONFunction(j);
    if (j.begin().key() == "CallFunction")
        return JSONCallFunction(j.begin().value());
    if (j.begin().key() == "Print")
        return JSONPrint(j.begin().value());
    if (j.begin().key() == "Block")
        return JSONBlock(j.begin().value());
    if (j.begin().key() == "Loop")
        return JSONLoop(j.begin().value());
    if (j.begin().key() == "Conditional")
        return JSONConditional(j.begin().value());
    if (j.begin().key() == "Object")
        return JSONObject(j.begin().value());
    if (j.begin().key() == "AssignField")
        return JSONAssignField(j.begin().value());
    if (j.begin().key() == "AccessField")
        return JSONAccessField(j.begin().value());
    if (j.begin().key() == "CallMethod")
        return JSONCallMethod(j.begin().value());
    throw ParserException();
}

ASTInteger * JSONToAST::JSONInteger(json & j)
{
    if (j.is_number_integer())
        return new ASTInteger(j);
    throw ParserException();
}

ASTBoolean * JSONToAST::JSONBoolean(json & j)
{
    if (j == true || j == false)
        return new ASTBoolean(j);
    throw ParserException();
}

ASTVariable * JSONToAST::JSONVariable(json & j)
{
    string name = j["name"];
    return new ASTVariable(name,convertSwitch(j["value"]));
}

ASTAccessVariable * JSONToAST::JSONAccessVariable(json & j)
{
    string name = j["name"];
    return new ASTAccessVariable(name);
}

ASTAssignVariable * JSONToAST::JSONAssignVariable(json & j)
{
    string name = j["name"];
    return new ASTAssignVariable(name,convertSwitch(j["value"]));
}

ASTArray * JSONToAST::JSONArray(json & j)
{
    json size = j["size"];
    json value = j["value"];
    return new ASTArray(convertSwitch(size),convertSwitch(value));
}

ASTAccessArray * JSONToAST::JSONAccessArray(json & j)
{
    json array = j["array"];
    json index = j["index"];
    return new ASTAccessArray(convertSwitch(array),convertSwitch(index));
}

ASTAssignArray * JSONToAST::JSONAssignArray(json & j)
{
    json array = j["array"];
    json index = j["index"];
    json value = j["value"];
    return new ASTAssignArray(convertSwitch(array),convertSwitch(index),convertSwitch(value));
}

ASTFunction * JSONToAST::JSONFunction(json & j)
{
    string name = j.begin().value()["name"];
    json param = j.begin().value()["parameters"];
    json body = j.begin().value()["body"];
    vector<string> params;
    for (auto it = param.begin(); it!= param.end(); it++)
    {
        params.push_back(*it);
    }
    string s = to_string(j);
    return new ASTFunction(name,params,convertSwitch(body),s);
}

ASTCallFunction * JSONToAST::JSONCallFunction(json & j)
{
    string name = j["name"];
    json arg = j["arguments"];
    vector<AST*> args;
    for (auto it = arg.begin(); it!= arg.end(); it++)
    {
        args.push_back(convertSwitch(*it));
    }
    return new ASTCallFunction(name,args);
}

ASTPrint * JSONToAST::JSONPrint(json & j)
{
    string format = j["format"];
    json arg = j["arguments"];
    vector<AST*> args;
    for (auto it = arg.begin(); it!= arg.end(); it++)
    {
        args.push_back(convertSwitch(*it));
    }
    return new ASTPrint(format,args);
}

ASTBlock * JSONToAST::JSONBlock(json & j)
{
    vector<AST*> stmt;
    for (auto it = j.begin(); it!= j.end(); it++)
    {
        stmt.push_back(convertSwitch(*it));
    }
    return new ASTBlock(stmt);

}

ASTTop * JSONToAST::JSONTop(json & j)
{
    vector<AST*> stmt;
    for (auto it = j.begin(); it!= j.end(); it++)
    {
        stmt.push_back(convertSwitch(*it));
    }
    return new ASTTop(stmt);
}

ASTLoop * JSONToAST::JSONLoop(json & j)
{
    json cond = j["condition"];
    json body = j["body"];
    return new ASTLoop(convertSwitch(cond),convertSwitch(body));
}

ASTConditional * JSONToAST::JSONConditional(json & j)
{
    json cond = j["condition"];
    json _if = j["consequent"];
    json _else = j["alternative"];
    return new ASTConditional(convertSwitch(cond),convertSwitch(_if),convertSwitch(_else));
}

ASTObject * JSONToAST::JSONObject(json & j)
{
    json extends = j["extends"];
    json member = j["members"];
    vector<AST*> members;
    for (auto it = member.begin(); it!= member.end(); it++)
    {
        members.push_back(convertSwitch(*it));
    }
    return new ASTObject(convertSwitch(extends),members);
}

ASTAssignField * JSONToAST::JSONAssignField(json & j)
{
    json object = j["object"];
    string field = j["field"];
    json value = j["value"];
    return new ASTAssignField(convertSwitch(object),field,convertSwitch(value));
}

ASTAccessField * JSONToAST::JSONAccessField(json & j)
{
    json object = j["object"];
    string field = j["field"];
    return new ASTAccessField(convertSwitch(object),field);
}

ASTCallMethod * JSONToAST::JSONCallMethod(json & j)
{
    json object = j["object"];
    string name = j["name"];
    json arg = j["arguments"];
    vector<AST*> args;
    for (auto it = arg.begin(); it!= arg.end(); it++)
    {
        args.push_back(convertSwitch(*it));
    }
    return new ASTCallMethod(convertSwitch(object),name,args);
}
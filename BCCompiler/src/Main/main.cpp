
#include <fstream>
#include <iostream>
#include "../Structures/AST.h"
#include "../JSON/JSONToAST.h"
#include "../Compiler/Compiler.hpp"
#include <memory>
#include <fstream>

using namespace std;

void WriteToFile(unsigned char * data, uint64_t size, char * name)
{
  ofstream file(name, std::ios::binary);
  file.write((const char *)data,size);
  file.close();
}

int main(int argc, char** argv)
{
  if (argc != 3){
    cout << "Wrong number of arguments" << endl;
    return 0;
  }

  ifstream file(argv[1]);

  JSONToAST converter;
  AST * ast = nullptr;

  try{
    ast = converter.convert(file);
  }catch(ParserException& e){
    cout << e.what() << endl;
    return 0;
  }

  shared_ptr<Compiler> compile = make_shared<Compiler>();

  try{
    ast->Accept(*compile.get());
  }catch(const string & e){
        cout << e << endl;
  }

  try{
    uint64_t size;
    unsigned char * code = compile->Final(size);
    WriteToFile(code,size,argv[2]);
    delete [] code;
  }catch(const string & e)
  {
        cout << e << endl;
  }
  
  delete ast;

  return 0;
}

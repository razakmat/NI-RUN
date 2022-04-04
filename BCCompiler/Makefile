CXX=g++
CXXFLAGS= -std=c++17 -Wall -pedantic -Wno-long-long -O3
CXXLIBS=
OBJECTS= ./obj/main.o ./obj/JSONToAST.o ./obj/Instruction.o ./obj/ProgramObject.o ./obj/utils.o ./obj/AST.o ./obj/Compiler.o

all: compile
	
compile: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o Compiler $(CXXLIBS)

./obj/%.o: ./src/*/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

./obj/main.o: ./src/Main/main.cpp ./src/Structures/AST.h ./src/JSON/JSONToAST.h ./src/Compiler/Compiler.hpp
./obj/JSONToAST.o: ./src/JSON/JSONToAST.cpp ./src/Structures/AST.h
./obj/Instruction.o: ./src/Structures/Instruction.cpp ./src/Utils/utils.hpp
./obj/ProgramObject.o: ./src/Structures/ProgramObject.cpp ./src/Structures/Instruction.hpp ./src/Utils/utils.hpp
./obj/utils.o: ./src/Utils/utils.cpp
./obj/AST.o: ./src/Structures/AST.cpp ./src/Visitor/Visitor.h
./obj/Compiler.o: ./src/Compiler/Compiler.cpp ./src/Utils/utils.hpp ./src/Structures/ProgramObject.hpp ./src/Structures/Instruction.hpp ./src/Visitor/Visitor.h ./src/Structures/AST.h

clean:	
	rm ./obj/*.o
	rm Compiler



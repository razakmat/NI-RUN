CXX=g++
CXXFLAGS= -std=c++17 -Wall -pedantic -Wno-long-long -O3
CXXLIBS=

all: compile
	
compile: ./src/main.o ./src/Interpreter.o ./src/Instruction.o ./src/ProgramObject.o ./src/utils.o ./src/Heap.o ./src/Parser.o ./src/FrameStack.o ./src/OpStack.o ./src/Dispatch.o
	$(CXX) $(CXXFLAGS) $^ -o interpreter $(CXXLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

./src/main.o: ./src/main.cpp ./src/Parser.hpp ./src/FMLVM.hpp ./src/Interpreter.hpp	./src/Heap.hpp
./src/Interpreter.o: ./src/Interpreter.cpp ./src/FMLVM.hpp ./src/Instruction.hpp ./src/OpStack.hpp ./src/Heap.hpp
./src/Instruction.o: ./src/Instruction.cpp
./src/ProgramObject.o: ./src/ProgramObject.cpp
./src/utils.o: ./src/utils.cpp
./src/Heap.o: ./src/Heap.cpp ./src/ProgramObject.hpp ./src/RuntimeObject.hpp ./src/utils.hpp ./src/Interpreter.hpp
./src/Parser.o: ./src/Parser.cpp ./src/FMLVM.hpp ./src/ProgramObject.hpp ./src/Instruction.hpp ./src/utils.hpp ./src/Heap.hpp
./src/OpStack.o: ./src/OpStack.cpp
./src/FrameStack.o: ./src/FrameStack.cpp
./src/Dispatch.o: ./src/Dispatch.cpp ./src/RuntimeObject.hpp ./src/FMLVM.hpp ./src/Heap.hpp

clean:	
	rm ./src/*.o 
	rm interpreter



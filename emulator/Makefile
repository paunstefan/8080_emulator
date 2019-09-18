CXX=g++
CFLAGS=-Wall -g
OBJ = main.cpp emulator.c disassemble.c SIMachine.cpp Display.cpp

emulator: $(OBJ)
	$(CXX) -o $@ $^ $(CFLAGS) -lSDL2

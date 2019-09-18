#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "emulator.h"
#include "SIMachine.hpp"

int main(int argc, char **argv){
	SIMachine machine;

	machine.start_emulation();

	return 0;
}
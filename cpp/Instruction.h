#ifndef CPP_MODULE_INSTRUCTION_H
#define CPP_MODULE_INSTRUCTION_H

#include <string>

enum class Instruction {
    WALK,
    JUMP,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    ROTATE_BACK,
};

std::string instructionToString(Instruction instruction);

#endif

//
// Created by hugo on 07-03-2024.
//
#include "Instruction.h"

std::string instructionToString(Instruction instruction) {
    switch (instruction) {
        case Instruction::WALK:
            return "WALK";
        case Instruction::JUMP:
            return "JUMP";
        case Instruction::ROTATE_LEFT:
            return "ROTATE_LEFT";
        case Instruction::ROTATE_RIGHT:
            return "ROTATE_RIGHT";
        case Instruction::ROTATE_BACK:
            return "ROTATE_BACK";
        default:
            return "UNKNOWN";
    }
}

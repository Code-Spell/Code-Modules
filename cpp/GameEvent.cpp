//
// Created by hugo on 07-03-2024.
//

#include "GameEvent.h"

std::string GameEvent::toString() {
    std::string statusAsString;

    if (status) {
        statusAsString = "true";
    } else {
        statusAsString = "false";
    }

    return instructionToString(instruction) + ":" +
        statusAsString + ":" +
        message + ":" +
        std::to_string(position.x) + "," + std::to_string(position.y) + "," + std::to_string(position.z);

}

GameEvent::GameEvent(Instruction instruction, bool status, const std::string &message, Position3D position3D) {
    this->instruction = instruction;
    this->status = status;
    this->message = message;
    this->position = position3D;
}

bool GameEvent::getStatus() const {
    return this->status;
}

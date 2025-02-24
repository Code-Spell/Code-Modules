#include <iostream>
#include <string>
#include "GameCharacter.h"
#include "cJSON.h"

GameCharacter::GameCharacter(GameRendererConnector &gameRendererConnector)
        : gameRendererConnector(gameRendererConnector) {

    this->currentPosition = {0, 0, 0};
    this->nextPosition = {0, 0};
    this->currentDirection = Direction::XPositive;

}

bool GameCharacter::walk() {
    GameEvent event = gameRendererConnector.sendInstruction(Instruction::WALK);
    instructions.emplace_back(event.toString());
    return event.getStatus();
}

bool GameCharacter::turnBackward() {
    GameEvent event = gameRendererConnector.sendInstruction(Instruction::ROTATE_BACK);
    instructions.emplace_back(event.toString());
    return event.getStatus();
}

bool GameCharacter::turnLeft() {
    GameEvent event = gameRendererConnector.sendInstruction(Instruction::ROTATE_LEFT);
    instructions.emplace_back(event.toString());
    return event.getStatus();
}

bool GameCharacter::turnRight() {
    GameEvent event = gameRendererConnector.sendInstruction(Instruction::ROTATE_RIGHT);
    instructions.emplace_back(event.toString());
    return event.getStatus();
}

bool GameCharacter::jump() {
    GameEvent event = gameRendererConnector.sendInstruction(Instruction::JUMP);
    instructions.emplace_back(event.toString());
    return event.getStatus();
}

void GameCharacter::apply() {
    exportInstructions();
}

Position3D GameCharacter::getCurrentPosition() {
    return currentPosition;
}

Position2D GameCharacter::getNextPosition() {
    return nextPosition;
}

Direction GameCharacter::getCurrentDirection() {
    return currentDirection;
}

void GameCharacter::updateCharacterStatus() {

    std::vector<char> jsonData = gameRendererConnector.obtainCharacterStatusData();

    char* jsonCharArray = new char[jsonData.size() + 1];
    std::copy(jsonData.begin(), jsonData.end(), jsonCharArray);
    jsonCharArray[jsonData.size()] = '\0';

    cJSON* root = cJSON_Parse(jsonCharArray);
    if (root == nullptr) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != nullptr) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return;
    }

    cJSON* parsedCharacter = cJSON_GetObjectItemCaseSensitive(root, "character");
    cJSON* parsedCurrentPosition = cJSON_GetObjectItemCaseSensitive(parsedCharacter, "current_position");
    cJSON* parsedNextPosition = cJSON_GetObjectItemCaseSensitive(parsedCharacter, "next_position");
    cJSON* parsedDirection = cJSON_GetObjectItemCaseSensitive(parsedCharacter, "direction");

    double currentX = cJSON_GetObjectItem(parsedCurrentPosition, "x")->valuedouble;
    double currentY = cJSON_GetObjectItem(parsedCurrentPosition, "y")->valuedouble;
    double currentZ = cJSON_GetObjectItem(parsedCurrentPosition, "z")->valuedouble;
    double nextX = cJSON_GetObjectItem(parsedNextPosition, "x")->valuedouble;
    double nextZ = cJSON_GetObjectItem(parsedNextPosition, "z")->valuedouble;

    currentPosition = {currentX, currentY, currentZ};
    nextPosition = {nextX, nextZ};

    std::string direction = cJSON_GetStringValue(parsedDirection);

    if (direction == "XPositive") {
        currentDirection = Direction::XPositive;
    } else if (direction == "XNegative") {
        currentDirection = Direction::XNegative;
    } else if (direction == "ZPositive") {
        currentDirection = Direction::ZPositive;
    } else if (direction == "ZNegative") {
        currentDirection = Direction::ZNegative;
    } else {
        currentDirection = Direction::UNKNOWN;
    }

    // cJSON cleanup
    cJSON_Delete(root);
    delete[] jsonCharArray;

}

void GameCharacter::exportInstructions() {

    std::ofstream file("/gamification/game_events.txt");

    for (const auto& instruction : instructions) {
        file << instruction << std::endl;
    }

    file.close();

}





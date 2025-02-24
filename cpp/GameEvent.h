#ifndef CPP_MODULE_GAMEEVENT_H
#define CPP_MODULE_GAMEEVENT_H

#include <string>
#include "Instruction.h"
#include "utils.h"


class GameEvent {

private:
    Instruction instruction;
    bool status{};
    std::string message;
    Position3D position{};

public:
    GameEvent (Instruction instruction, bool status, const std::string& message, Position3D position3D);

    [[nodiscard]] bool getStatus() const;
    std::string toString();
    
};


#endif

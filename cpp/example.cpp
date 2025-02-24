#include "GameRendererConnector.h"
#include "GameCharacter.h"
#include <cstring>

// Define your SpellyController class here
class SpellyController {

private:
    GameCharacter gameCharacter;
    Position3D position;

public:

    SpellyController(GameCharacter gameCharacter);
    void walk();
    void jump();
    void rotate(std::string direction);

};

// Implement the SpellyController class methods here

SpellyController::SpellyController(GameCharacter gameCharacter) : gameCharacter(std::move(gameCharacter)) { }

void SpellyController::walk() {
    this->gameCharacter.walk();
    std::cout << "Walking to the indicated position" << std::endl;
}

void SpellyController::jump() {
    this->gameCharacter.jump();
}

void SpellyController::rotate(std::string direction) {
    if (direction == "LEFT") {
        this->gameCharacter.turnLeft();
    } else if (direction == "RIGHT") {
        this->gameCharacter.turnRight();
    } else if (direction == "BACK") {
        this->gameCharacter.turnBackward();
    }
}

int main() {

    // Only one instance of GameRendererConnector is allowed
    GameRendererConnector gameRendererConnector = GameRendererConnector();
    GameCharacter gameCharacter = GameCharacter(gameRendererConnector);

    // Instantiate your SpellyController class. Provide gameCharacter object in your constructor.
    SpellyController controller = SpellyController(gameCharacter);

    // Create code to move Spelly to the indicated position
    gameCharacter.walk();

    // Apply movements
    gameCharacter.apply();

    return 0;

}


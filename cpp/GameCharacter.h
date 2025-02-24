#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <vector>
#include "GameRendererConnector.h"
#include "utils.h"

class GameCharacter {

    public:

        explicit GameCharacter(GameRendererConnector &gameRendererConnector);

        bool walk();
        bool turnBackward();
        bool turnLeft();
        bool turnRight();
        bool jump();

        Position3D getCurrentPosition();
        Position2D getNextPosition();
        Direction getCurrentDirection();

        void updateCharacterStatus();

        void apply();

    private:

        std::list<std::string> instructions;
        GameRendererConnector& gameRendererConnector;

        Position3D currentPosition{};
        Position2D nextPosition{};
        Direction currentDirection{};

        void exportInstructions();

};


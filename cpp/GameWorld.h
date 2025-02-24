#ifndef CPP_MODULE_GAMEWORLD_H
#define CPP_MODULE_GAMEWORLD_H

#include <map>
#include "utils.h"
#include "GameRendererConnector.h"

class GameWorld {

    private:
        GameRendererConnector& gameRendererConnector;
        std::map<Position2D, double> heightMap;

    public:

        explicit GameWorld(GameRendererConnector &gameRendererConnector);

        void loadWorld();

        double getHeightAt(Position2D position);

        std::map<Position2D, double> getHeightMap();

        ~GameWorld();

};


#endif

#include "GameWorld.h"
#include "cJSON.h"


GameWorld::GameWorld(GameRendererConnector &gameRendererConnector)
        : gameRendererConnector(gameRendererConnector) { }

void GameWorld::loadWorld() {

    std::vector<char> worldInfo = gameRendererConnector.obtainWorldInfoData();

    // Convert std::vector<char> to char array for cJSON
    char* jsonCharArray = new char[worldInfo.size() + 1];
    std::copy(worldInfo.begin(), worldInfo.end(), jsonCharArray);
    jsonCharArray[worldInfo.size()] = '\0'; // Null-terminate the string

    // Parse JSON data
    cJSON* root = cJSON_Parse(jsonCharArray);
    if (root == nullptr) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != nullptr) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return; // Or handle error as needed
    }

    // Access "world" and "blocks"
    cJSON* world = cJSON_GetObjectItemCaseSensitive(root, "world");
    cJSON* blocks = cJSON_GetObjectItemCaseSensitive(world, "blocks");
    cJSON* forestObjects = cJSON_GetObjectItemCaseSensitive(world, "forest");
    cJSON* extraMeshObjects = cJSON_GetObjectItemCaseSensitive(world, "extra_meshes");

    // Iterate over "blocks" and populate heightMap
    cJSON* block = nullptr;
    cJSON_ArrayForEach(block, blocks) {
        cJSON* positionX = cJSON_GetArrayItem(block, 0);
        cJSON* positionZ = cJSON_GetArrayItem(block, 1);
        cJSON* height = cJSON_GetArrayItem(block, 2);

        Position2D pos = {positionX->valuedouble, positionZ->valuedouble};
        double heightValue = height->valuedouble;
        if (heightMap.find(pos) == heightMap.end() || heightMap[pos] < heightValue) {
            heightMap[pos] = heightValue;
        }
    }

    // Iterate over "forestObjects" and populate heightMap
    cJSON* forestObject = nullptr;
    cJSON_ArrayForEach(forestObject, forestObjects) {
        cJSON *positionX = cJSON_GetArrayItem(forestObject, 0);
        cJSON *positionZ = cJSON_GetArrayItem(forestObject, 1);
        cJSON *height = cJSON_GetArrayItem(forestObject, 2);

        Position2D pos = {positionX->valuedouble, positionZ->valuedouble};
        double heightValue = height->valuedouble;
        if (heightMap.find(pos) == heightMap.end() || heightMap[pos] < heightValue) {
            heightMap[pos] = heightValue;
        }
    }

    // Iterate over "extraMeshObjects" and populate heightMap
    cJSON* extraMeshObject = nullptr;
    cJSON_ArrayForEach(extraMeshObject, extraMeshObjects) {
        cJSON *positionX = cJSON_GetArrayItem(extraMeshObject, 0);
        cJSON *positionZ = cJSON_GetArrayItem(extraMeshObject, 1);
        cJSON *height = cJSON_GetArrayItem(extraMeshObject, 2);

        Position2D pos = {positionX->valuedouble, positionZ->valuedouble};
        double heightValue = height->valuedouble;
        if (heightMap.find(pos) == heightMap.end() || heightMap[pos] < heightValue) {
            heightMap[pos] = heightValue;
        }
    }

    // Cleanup
    cJSON_Delete(root);
    delete[] jsonCharArray;

}

double GameWorld::getHeightAt(Position2D position) {
    return heightMap[position];
}

std::map<Position2D, double> GameWorld::getHeightMap() {
    return this->heightMap;
}

GameWorld::~GameWorld() = default;

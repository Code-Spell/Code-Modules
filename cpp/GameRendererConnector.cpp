#include <thread>
#include "GameRendererConnector.h"
#include "cJSON.h"
#include <netdb.h>

GameRendererConnector::GameRendererConnector() {
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(7777);

    struct hostent *server = gethostbyname(std::getenv("GAME_RENDERER_HOST"));

    if (server == nullptr) {
        std::cerr << "Error connecting to Game Renderer server - Unable to resolve hostname." << std::endl;
        exit(32);
    }

    bcopy((char *)server->h_addr, (char *)&serverAddress.sin_addr.s_addr, server->h_length);

    if (connectToServer() != 0) {
        std::cerr << "Error connecting to Game Renderer server - No connection established." << std::endl;
        exit(33);
    }

}

int GameRendererConnector::connectToServer() {

    int successfulConnection = -1;
    int retries = 0;

    while (retries < 3 && successfulConnection != 0) {

        successfulConnection = connect(clientSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

        if (successfulConnection != 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            retries++;
        }

    }

    return successfulConnection;

}

GameEvent GameRendererConnector::sendInstruction(Instruction instruction) const {

    std::vector<char> message = obtainMessageFromInstruction(instruction);
    std::vector<char> response(4096);

    // Send instruction to TCP Server - GameRenderer
    write(clientSocket, message.data(), message.size());

    // Read game status from TCP Server - GameRenderer
    // Ignore warning

    read(clientSocket, response.data(), response.size());

    char* jsonCharArray = new char[response.size() + 1];
    std::copy(response.begin(), response.end(), jsonCharArray);
    jsonCharArray[response.size()] = '\0'; // Null-terminate the string

    cJSON* root = cJSON_Parse(jsonCharArray);
    if (root == nullptr) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != nullptr) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return {instruction, false, "ERROR", Position3D{}};
    }

    cJSON* statusItem = cJSON_GetObjectItem(root, "status");
    cJSON* messageItem = cJSON_GetObjectItem(root, "message");
    cJSON* positionItem = cJSON_GetObjectItem(root, "position");

    if (statusItem == nullptr || cJSON_IsBool(statusItem) == false) {
        std::cerr << "Error parsing JSON response from Game Renderer - 'status' field not found or not boolean." << std::endl;
        return {instruction, false, "ERROR", Position3D{}};
    }

    if (messageItem == nullptr || cJSON_IsString(messageItem) == false) {
        std::cerr << "Error parsing JSON response from Game Renderer - 'message' field not found or not string." << std::endl;
        return {instruction, false, "ERROR", Position3D{}};
    }

    if (positionItem == nullptr) {
        std::cerr << "Error parsing JSON response from Game Renderer - 'position' field not found." << std::endl;
        return {instruction, false, "ERROR", Position3D{}};
    }

    bool status = cJSON_IsTrue(statusItem);
    std::string messageString = cJSON_GetStringValue(messageItem);

    double x = cJSON_GetObjectItem(positionItem, "x")->valuedouble;
    double y = cJSON_GetObjectItem(positionItem, "y")->valuedouble;
    double z = cJSON_GetObjectItem(positionItem, "z")->valuedouble;

    // Cleanup
    cJSON_Delete(root);
    delete[] jsonCharArray;

    return {instruction, status, messageString, {x, y, z}};

}

std::vector<char> GameRendererConnector::obtainWorldInfoData() const {

    // The world info message must not be bigger than 97670 bytes - fits in 5 bytes
    std::vector<char> worldInfoMessageSizeBuffer(5);
    std::vector<char> worldInfoBuffer(97670);

    /**
     * Because the world info is a HUGE message, sometimes it is not possible to read it all at once.
     * In this case, we need to first obtain the size of the message, check if we received everything and, if not,
     * read the rest of the message.
     */
    // Here we request to the game renderer the size of the world info message
    write(clientSocket, "world_info_size", 15);

    // Here we read the size of the message
    read(clientSocket, worldInfoMessageSizeBuffer.data(), 6);

    // We convert the buffer to an integer
    int worldInfoMessageSize = std::stoi(std::string(worldInfoMessageSizeBuffer.begin(), worldInfoMessageSizeBuffer.end()));

    write(clientSocket, "world_info", 10);

    // Here we read the world info message and check how many bytes we have actually read
    long totalBytesRead = read(clientSocket, worldInfoBuffer.data(), worldInfoMessageSize);

    while (totalBytesRead < worldInfoMessageSize) {
        long currentBytesRead = read(clientSocket, worldInfoBuffer.data() + totalBytesRead, worldInfoMessageSize - totalBytesRead);
        totalBytesRead += currentBytesRead;
    }

    return worldInfoBuffer;


}

std::vector<char> GameRendererConnector::obtainCharacterStatusData() const {

    std::vector<char> characterStatus(4096);

    // Send instruction to TCP Server - GameRenderer
    write(clientSocket, "character_status", 16);

    // Read game status from TCP Server - GameRenderer
    read(clientSocket, characterStatus.data(), characterStatus.size());

    return characterStatus;

}

GameRendererConnector::~GameRendererConnector() {
    close(clientSocket);
}

std::vector<char> GameRendererConnector::obtainMessageFromInstruction(Instruction instruction) {
    std::string message = instructionToString(instruction);
    return {message.begin(), message.end()};
}
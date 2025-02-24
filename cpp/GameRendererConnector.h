#ifndef TCP_CONNECTION_GAMERENDERERCONNECTOR_H
#define TCP_CONNECTION_GAMERENDERERCONNECTOR_H

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include "GameEvent.h"

class GameRendererConnector {

    private:
        int clientSocket;
        sockaddr_in serverAddress{};

        int connectToServer();

    public:

        GameRendererConnector();

        [[nodiscard]] GameEvent sendInstruction(Instruction instruction) const;

        [[nodiscard]] std::vector<char> obtainWorldInfoData() const;

        [[nodiscard]] std::vector<char> obtainCharacterStatusData() const;

        ~GameRendererConnector();

    private:

        static std::vector<char> obtainMessageFromInstruction(Instruction instruction);

};

#endif

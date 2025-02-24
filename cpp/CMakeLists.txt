cmake_minimum_required(VERSION 3.27)
project(CPP_Module)

set(CMAKE_CXX_STANDARD 17)

add_executable(CPP_Module main.cpp GameRendererConnector.cpp GameCharacter.cpp
        GameWorld.cpp
        GameWorld.h
        utils.h
        cJSON.cpp
        GameEvent.cpp
        GameEvent.h
        Instruction.h
        Instruction.cpp)
cmake_minimum_required(VERSION 3.17)
project(DB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND SOURCES
        main.cpp
        DataBase.cpp
        GameStudio.cpp Game.cpp)

list(APPEND HEADERS
        DataBase.h
        Game.h
        GameStudio.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
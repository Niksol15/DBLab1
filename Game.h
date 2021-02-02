//
// Created by solon on 31.01.2021.
//

#ifndef DB_GAME_H
#define DB_GAME_H
#include <iostream>

constexpr int kGameStringMaxSize = 50;

struct Game {
    int id = -1;
    bool isDel = false;
    int nextSlave = -1;
    char name[kGameStringMaxSize] = "";
    char genre[kGameStringMaxSize] = "";
};

std::ostream& operator<< (std::ostream& os, const Game& game);
#endif //DB_GAME_H

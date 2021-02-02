//
// Created by solon on 31.01.2021.
//

#ifndef DB_GAMESTUDIO_H
#define DB_GAMESTUDIO_H

#include <iostream>

constexpr int kGSStringMaxSize = 50;

struct GameStudio{
    int id = -1;
    bool isDel = false;
    int firstSlave = -1;
    int slaveCounter = 0;
    char name[kGSStringMaxSize] = "";
    char location[kGSStringMaxSize] = "";
};

std::ostream& operator<<(std::ostream& os, const GameStudio& studio);

#endif //DB_GAMESTUDIO_H

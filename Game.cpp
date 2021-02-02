//
// Created by solon on 01.02.2021.
//
#include "Game.h"

std::ostream &operator<<(std::ostream &os, const Game &game) {
    os << "id: " << game.id << "\n";
    os << "is delete: " << game.isDel << "\n";
    os << "next slave: " << game.nextSlave << "\n";
    os << "name: " << game.name << "\n";
    os << "genre: " << game.genre;
    return os;
}

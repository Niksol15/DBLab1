//
// Created by solon on 01.02.2021.
//
#include "GameStudio.h"

std::ostream &operator<<(std::ostream &os, const GameStudio &studio) {
    os << "id: " << studio.id << "\n";
    os << "is deleted: " << studio.isDel << "\n";
    os << "first slave: " << studio.firstSlave << "\n";
    os << "slave counter: " << studio.slaveCounter << "\n";
    os << "name: " << studio.name << "\n";
    os << "location: " << studio.location;
    return os;
}

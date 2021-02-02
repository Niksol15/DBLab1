#include <iostream>
#include "DataBase.h"

int main() {
    DataBase dataBase;
    std::string command;
    while (std::getline(std::cin, command) && dataBase.ProcessCommand(command) != DBCommandType::EXIT) {}
    return 0;
}

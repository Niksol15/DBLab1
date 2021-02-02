//
// Created by solon on 31.01.2021.
//

#include "DataBase.h"
#include <sstream>
#include <algorithm>
#include <cstring>

using namespace std;

namespace {
    constexpr char kMasterFileName[] = "GameStudio.fl";
    constexpr char kMasterIndexName[] = "GameStudio.ind";
    constexpr char kSlaveFileName[] = "Game.fl";
    constexpr char kLogFileName[] = "log.txt";
} //DataBaseConst

namespace {
    constexpr char kGET_MStr[] = "get-m";
    constexpr char kGET_SStr[] = "get-s";
    constexpr char kDEL_MStr[] = "del-m";
    constexpr char kDEL_SStr[] = "del-s";
    constexpr char kUPDATE_MStr[] = "update-m";
    constexpr char kUPDATE_SStr[] = "update-s";
    constexpr char kINSERT_MStr[] = "insert-m";
    constexpr char kINSERT_SStr[] = "insert-s";
    constexpr char kCALC_MStr[] = "calc-m";
    constexpr char kCALC_SStr[] = "calc-s";
    constexpr char kUT_MStr[] = "ut-m";
    constexpr char kUT_SStr[] = "ut-s";
    constexpr char kEXITStr[] = "exit";
} //CommandTypeStrConst

namespace {
    constexpr char WrongFormatError[] = "Wrong command format\n";
    constexpr char BadIndex[] = "Something wrong with index file\n";
    constexpr char KeyDuplicateInsertError[] = "Key duplicates detected while insert, try again\n";
    constexpr char KeyDuplicateConstructorError[] = "Key duplicates detected in constructor, fatal error\n";
    constexpr char UndefinedCalled[] = "Undefined method called\n";
    //constexpr char WrongMatchingMethod[] = "Wrong matching method\n";
    constexpr char CharLiteralIsTooLongError[] = "Char literal is too long, max length 50\n";
    constexpr char MasterDoesntExist[] = " Master with this id doesn`t exist\n";
    constexpr char SlaveDoesntExist[] = " Slave with this id doesn`t exist for this master\n";
    constexpr char KeyDuplicateSlave[] = " Slave with this id already exists\n";
    constexpr char SlaveDeleted[] = "Slave successfully deleted\n";
    constexpr char NonExistingField[] = "Non existing field";
} //Help

DataBase::DataBase() : masterIndex(kMasterIndexName, ios::binary | ios::in | ios::out),
                       masterFile(kMasterFileName, ios::binary | ios::in | ios::out),
                       slaveFile(kSlaveFileName, ios::binary | ios::in | ios::out),
                       log(kLogFileName) {
    if (!masterIndex) {
        cout << BadIndex;
        exit(0);
    }
    while (masterIndex.peek() != EOF) {
        int key, pos;
        masterIndex.read(reinterpret_cast<char *>(&key), sizeof(int));
        masterIndex.read(reinterpret_cast<char *>(&pos), sizeof(int));
        if (idToPos.count(key)) {
            cout << KeyDuplicateConstructorError << endl;
            exit(0);
        }
        idToPos[key] = pos;
    }
}

//TODO Add shrinkToFit
DataBase::~DataBase() {
    if (masterIndex.bad()) {
        cout << "Problem with saving\n";
        return;
    }
    rewriteIndex();
    cleanMasterFl();
    cleanSlaveFl();
}

void DataBase::rewriteIndex() {
    masterIndex.clear();
    masterIndex.seekg(0, ios::beg);
    for (auto[ckey, value]: idToPos) {
        int key = ckey;
        masterIndex.write(reinterpret_cast<char *>(&key), sizeof(key));
        masterIndex.write(reinterpret_cast<char *>(&value), sizeof(value));
    }
}

DBCommandType DataBase::ProcessCommand(const string &command) {
    std::string strCommand = command.substr(0, command.find(' '));
    if (strCommand == kGET_MStr) {
        log << kGET_MStr << " called\n";
        GetM(command);
        return DBCommandType::GET_M;
    } else if (strCommand == kGET_SStr) {
        log << kGET_SStr << " called\n";
        GetS(command);
        return DBCommandType::GET_S;
    } else if (strCommand == kDEL_MStr) {
        log << kDEL_MStr << " called\n";
        DelM(command);
        return DBCommandType::DEL_M;
    } else if (strCommand == kDEL_SStr) {
        log << kDEL_SStr << " called\n";
        DelS(command);
        return DBCommandType::DEL_S;
    } else if (strCommand == kUPDATE_MStr) {
        log << kUPDATE_MStr << " called\n";
        UpdateM(command);
        return DBCommandType::UPDATE_M;
    } else if (strCommand == kUPDATE_SStr) {
        log << kUPDATE_SStr << " called\n";
        UpdateS(command);
        return DBCommandType::UPDATE_S;
    } else if (strCommand == kINSERT_MStr) {
        log << kINSERT_MStr << " called\n";
        InsertM(command);
        return DBCommandType::INSERT_M;
    } else if (strCommand == kINSERT_SStr) {
        log << kINSERT_SStr << " called\n";
        InsertS(command);
        return DBCommandType::INSERT_S;
    } else if (strCommand == kCALC_MStr) {
        log << kCALC_MStr << " called\n";
        CalcM();
        return DBCommandType::CALC_M;
    } else if (strCommand == kCALC_SStr) {
        log << kCALC_SStr << " called\n";
        CalcS();
        return DBCommandType::CALC_S;
    } else if (strCommand == kUT_MStr) {
        log << kUT_MStr << " called\n";
        UtM();
        return DBCommandType::UT_M;
    } else if (strCommand == kUT_SStr) {
        log << kUT_SStr << " called\n";
        UtS();
        return DBCommandType::UT_S;
    } else if (strCommand == kEXITStr) {
        log << kEXITStr << " called\n";
        return DBCommandType::EXIT;
    } else {
        log << UndefinedCalled;
        cout << UndefinedCalled;
        return DBCommandType::UNDEFINED;
    }
}

void DataBase::InsertM(const std::string &command) {
    size_t spaceCount = count(command.begin(), command.end(), ' ');
    if (spaceCount != 3) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(numeric_limits<streamsize>::max(), ' ');
    int id;
    is >> id;
    string name, location;
    is >> name >> location;
    if (name.size() >= kGSStringMaxSize || location.size() >= kGSStringMaxSize) {
        std::cout << CharLiteralIsTooLongError;
        return;
    }
    InsertM(id, name, location);
}

void DataBase::InsertM(int id, const std::string &name, const std::string &location) {
    if (idToPos.count(id)) {
        std::cout << KeyDuplicateInsertError;
        return;
    }
    int pos;
    if (!deletedMasters.empty()) {
        pos = deletedMasters.front();
        deletedMasters.pop();
    } else {
        masterFile.seekp(0, ios::end);
        pos = masterFile.tellp();
    }
    masterFile.seekp(pos);
    if (masterFile.bad()) {
        cout << "Fucking shit, fucking fstream motherfucker fail in insert-m\n";
        return;
    }
    idToPos[id] = pos;
    GameStudio gameStudio;
    gameStudio.id = id;
    strcpy(gameStudio.name, name.c_str());
    strcpy(gameStudio.location, location.c_str());
    masterFile.write(reinterpret_cast<char *>(&gameStudio), sizeof(gameStudio));
    cout << "Successful inserted " << id << " in " << pos << "\n";
}

void DataBase::GetM(const std::string &command) {
    int spaceCount = std::count(command.begin(), command.end(), ' ');
    if (spaceCount != 1) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int id;
    is >> id;
    GetM(id);
}

void DataBase::GetM(int id) {
    if (!idToPos.count(id)) {
        cout << id << MasterDoesntExist;
        return;
    }
    int pos = idToPos[id];
    masterFile.seekg(pos);
    if (masterFile.bad()) {
        cout << "Fucking shit, fucking fstream motherfucker fail at get-m\n";
        return;
    }
    GameStudio studio;
    masterFile.read(reinterpret_cast<char *>(&studio), sizeof(studio));
    cout << studio << "\n";
}

void DataBase::DelM(const std::string &command) {
    int spaceCount = std::count(command.begin(), command.end(), ' ');
    if (spaceCount != 1) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int id;
    is >> id;
    DelM(id);
}

void DataBase::DelM(int id) {
    if (!idToPos.count(id)) {
        cout << id << MasterDoesntExist;
        return;
    }
    int pos = idToPos[id];
    masterFile.seekp(pos + sizeof(int));
    if (masterFile.fail()) {
        cout << "Fucking stream doesn`t work\n";
        return;
    }
    bool del = true;
    masterFile.write(reinterpret_cast<char *>(&del), sizeof(bool));
    masterFile.seekg(pos + 2 * sizeof(int));
    int slaveIterator;
    masterFile.read(reinterpret_cast<char *>(&slaveIterator), sizeof(int));
    int numOfSlaves;
    masterFile.read(reinterpret_cast<char *>(&numOfSlaves), sizeof(int));
    cout << "Deleted id pointer to slave: " << slaveIterator << " num of slaves: " << numOfSlaves << "\n";
    for (int i = 0; i < numOfSlaves; ++i) {
        slaveFile.seekp(slaveIterator + sizeof(int));
        int s_id;
        slaveFile.seekg(slaveIterator);
        slaveFile.read(reinterpret_cast<char *>(&s_id), sizeof(int));
        deletedSlaves.push(s_id);
        slaveFile.write(reinterpret_cast<char *>(&del), sizeof(bool));
        slaveFile.seekg(slaveIterator + 2 * sizeof(int));
        slaveFile.read(reinterpret_cast<char * >(&slaveIterator), sizeof(int));
    }
    deletedMasters.push(pos);
    idToPos.erase(id);
    cout << "SuccessFul deleted\n";
}

void DataBase::InsertS(const std::string &command) {
    size_t spaceCount = count(command.begin(), command.end(), ' ');
    if (spaceCount != 4) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int m_id, s_id;
    is >> m_id >> s_id;
    string name, genre;
    is >> name >> genre;
    if (name.size() >= kGameStringMaxSize || genre.size() >= kGameStringMaxSize) {
        cout << CharLiteralIsTooLongError;
        return;
    }
    InsertS(m_id, s_id, name, genre);
}

void DataBase::InsertS(int m_id, int s_id, const std::string &name, const std::string &genre) {
    if (!idToPos.count(m_id)) {
        cout << m_id << MasterDoesntExist;
        return;
    }
    if (slaveFile.bad()) {
        cout << "Fucking fuck, slaveFile bad at inert-s\n";
        exit(0);
    }
    int m_pos = idToPos[m_id];
    int slaveIterator;
    masterFile.seekg(m_pos + 2 * sizeof(int));
    masterFile.read(reinterpret_cast<char *>(&slaveIterator), sizeof(int));
    int slaveNum;
    masterFile.read(reinterpret_cast<char *>(&slaveNum), sizeof(int));
    if (isSlaveAlreadyExist(slaveIterator, slaveNum, s_id)) {
        cout << KeyDuplicateSlave;
        return;
    }
    Game game;
    game.id = s_id;
    strcpy(game.name, name.c_str());
    strcpy(game.genre, genre.c_str());
    game.nextSlave = slaveIterator;
    int s_pos;
    if (!deletedSlaves.empty()) {
        s_pos = deletedSlaves.front();
        deletedSlaves.pop();
    } else {
        slaveFile.seekp(0, ios::end);
        s_pos = slaveFile.tellp();
    }
    masterFile.seekp(m_pos + 2 * sizeof(int));
    masterFile.write(reinterpret_cast<char * >(&s_pos), sizeof(int));
    ++slaveNum;
    masterFile.write(reinterpret_cast<char * >(&slaveNum), sizeof(int));
    slaveFile.seekp(s_pos);
    slaveFile.write(reinterpret_cast<char * >(&game), sizeof(Game));
    cout << "Successful inserted id: " << s_id << " pos: " << s_pos << endl;
}

bool DataBase::isSlaveAlreadyExist(int slaveIterator, int slaveNum, int s_id) {
    for (int i = 0; i < slaveNum && slaveIterator != -1; ++i) {
        int curr_id;
        slaveFile.seekg(slaveIterator);
        slaveFile.read(reinterpret_cast<char *>(&curr_id), sizeof(int));
        if (curr_id == s_id) {
            return true;
        }
        slaveFile.seekg(slaveIterator + 2 * sizeof(int));
        slaveFile.read(reinterpret_cast<char *>(&slaveIterator), sizeof(int));
    }
    return false;
}

void DataBase::UtM() {
    masterFile.seekg(0);
    while (masterFile.peek() != EOF) {
        if (masterFile.fail()) {
            cout << "Fucking fuck, slaveFile bad at ut-s\n";
            exit(0);
        }
        GameStudio studio;
        masterFile.read(reinterpret_cast<char *>(&studio), sizeof(GameStudio));
        cout << studio << "\n\n";
    }
}

void DataBase::UtS() {
    slaveFile.seekg(0);
    while (slaveFile.peek() != EOF) {
        if (slaveFile.bad()) {
            cout << "Fucking fuck, slaveFile bad at ut-s\n";
            exit(0);
        }
        Game game;
        slaveFile.read(reinterpret_cast<char *>(&game), sizeof(Game));
        cout << game << "\n\n";
    }
}

void DataBase::CalcM() {
    cout << "Master size: " << idToPos.size();
}

void DataBase::CalcS() {
    int res = 0;
    for (auto[key, value]: idToPos) {
        masterFile.seekg(key + 3 * sizeof(int));
        int curSlaveCount;
        masterFile.read(reinterpret_cast<char *>(&curSlaveCount), sizeof(int));
        res += curSlaveCount;
    }
    cout << "Slave size: " << res;
}

void DataBase::GetS(const std::string &command) {
    size_t spaceCount = count(command.begin(), command.end(), ' ');
    if (spaceCount != 2) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int m_id, s_id;
    is >> m_id >> s_id;
    GetS(m_id, s_id);
}


void DataBase::GetS(int m_id, int s_id) {
    int s_pos = getSlavePosById(m_id, s_id);
    if (s_pos == -1) {
        cout << SlaveDoesntExist;
        return;
    }
    slaveFile.seekg(s_pos);
    if (slaveFile.bad()) {
        cout << "Fucking fuck, slaveFile bad at get-s\n";
        return;
    }
    Game game;
    slaveFile.read(reinterpret_cast<char *>(&game), sizeof(Game));
    cout << game << "\n";
}

int DataBase::getSlavePosById(int m_id, int s_id) {
    if (!idToPos.count(m_id)) {
        cout << MasterDoesntExist;
        return -1;
    }
    int pos = idToPos[m_id];
    masterFile.seekg(pos + 2 * sizeof(int));
    int slaveIterator, slaveNum;
    masterFile.read(reinterpret_cast<char *>(&slaveIterator), sizeof(int));
    masterFile.read(reinterpret_cast<char *>(&slaveNum), sizeof(int));
    for (int i = 0; i < slaveNum && slaveIterator != -1; ++i) {
        int curr_id;
        slaveFile.seekg(slaveIterator);
        slaveFile.read(reinterpret_cast<char *>(&curr_id), sizeof(int));
        if (curr_id == s_id) {
            return slaveIterator;
        }
        slaveFile.seekg(slaveIterator + 2 * sizeof(int));
        slaveFile.read(reinterpret_cast<char *>(&slaveIterator), sizeof(int));
    }
    return -1;
}

void DataBase::DelS(const std::string &command) {
    size_t spaceCount = count(command.begin(), command.end(), ' ');
    if (spaceCount != 2) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int m_id, s_id;
    is >> m_id >> s_id;
    DelS(m_id, s_id);
}

void DataBase::DelS(int m_id, int s_id) {
    if (!idToPos.count(m_id)) {
        cout << MasterDoesntExist;
        return;
    }
    int m_pos = idToPos[m_id];
    masterFile.seekg(m_pos + 2 * sizeof(int));
    int firstSlavePos, slaveNum;
    masterFile.read(reinterpret_cast<char *>(&firstSlavePos), sizeof(int));
    masterFile.read(reinterpret_cast<char *>(&slaveNum), sizeof(int));
    int s_pos = getSlavePosById(m_id, s_id);
    if (s_pos == -1) {
        cout << SlaveDoesntExist;
        return;
    }
    decrementSlave(m_id);
    int isDel = true;
    slaveFile.seekp(s_pos + sizeof(int));
    slaveFile.write(reinterpret_cast<char *>(&isDel), sizeof(bool));
    deletedSlaves.push(s_pos);
    int nextSlavePos;
    slaveFile.seekg(s_pos + 2 * sizeof(int));
    slaveFile.read(reinterpret_cast<char *>(&nextSlavePos), sizeof(int));
    if (s_pos == firstSlavePos) {
        masterFile.seekp(m_pos + 2 * sizeof(int));
        masterFile.write(reinterpret_cast<char *>(&nextSlavePos), sizeof(int));
        cout << SlaveDeleted;
        return;
    }
    int slaveIterator = firstSlavePos;
    int nextSlaveIterator;
    for (int i = 0; i < slaveNum && slaveIterator != -1; ++i) {
        slaveFile.seekg(slaveIterator + 2 * sizeof(int));
        slaveFile.read(reinterpret_cast<char *>(&nextSlaveIterator), sizeof(int));
        if (nextSlaveIterator == s_pos) {
            slaveFile.seekp(slaveIterator + 2 * sizeof(int));
            slaveFile.write(reinterpret_cast<char *>(&nextSlavePos), sizeof(int));
            cout << SlaveDeleted;
            return;
        }
    }
    cout << "Something go wrong, LOL\n";
}

void DataBase::decrementSlave(int m_id) {
    if (!idToPos.count(m_id)) {
        cout << "Try to delete slave at doesnt exist master";
        return;
    }
    int pos = idToPos[m_id];
    masterFile.seekg(pos + 3 * sizeof(int));
    int slaveCount;
    masterFile.read(reinterpret_cast<char *>(&slaveCount), sizeof(int));
    --slaveCount;
    masterFile.seekp(pos + 3 * sizeof(int));
    masterFile.write(reinterpret_cast<char *>(&slaveCount), sizeof(int));
}

void DataBase::UpdateM(const std::string &command) {
    size_t spaceCount = count(command.begin(), command.end(), ' ');
    if (spaceCount != 3) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int id;
    is >> id;
    string fieldName, newValue;
    is >> fieldName >> newValue;
    if (newValue.size() >= kGSStringMaxSize) {
        cout << CharLiteralIsTooLongError;
        return;
    }
    if (fieldName == "name") {
        updateMasterName(id, newValue);
        return;
    } else if (fieldName == "location") {
        updateMasterLocation(id, newValue);
        return;
    }
    cout << NonExistingField;
}

void DataBase::updateMasterName(int id, const std::string &value) {
    if (!idToPos.count(id)) {
        cout << MasterDoesntExist;
        return;
    }
    int pos = idToPos[id];
    char newCharValue[kGSStringMaxSize];
    strcpy(newCharValue, value.c_str());
    masterFile.seekp(pos + 4 * sizeof(int));
    masterFile.write(newCharValue, sizeof(newCharValue));
    cout << "Successfully updated\n";
}

void DataBase::updateMasterLocation(int id, const std::string &value) {
    if (!idToPos.count(id)) {
        cout << MasterDoesntExist;
        return;
    }
    int pos = idToPos[id];
    char newCharValue[kGSStringMaxSize];
    strcpy(newCharValue, value.c_str());
    masterFile.seekp(pos + 4 * sizeof(int) + kGSStringMaxSize);
    masterFile.write(newCharValue, sizeof(newCharValue));
    cout << "Successfully updated\n";
}

void DataBase::UpdateS(const std::string &command) {
    size_t spaceCount = count(command.begin(), command.end(), ' ');
    if (spaceCount != 4) {
        cout << WrongFormatError;
        return;
    }
    istringstream is(command);
    is.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
    int m_id, s_id;
    is >> m_id >> s_id;
    string fieldName, newValue;
    is >> fieldName >> newValue;
    if (newValue.size() >= kGameStringMaxSize) {
        cout << CharLiteralIsTooLongError;
        return;
    }
    if (fieldName == "name") {
        updateSlaveName(m_id, s_id, newValue);
        return;
    } else if (fieldName == "genre") {
        updateSlaveGenre(m_id, s_id, newValue);
        return;
    }
    cout << NonExistingField;
}

void DataBase::updateSlaveName(int m_id, int s_id, const std::string &value) {
    if (!idToPos.count(m_id)) {
        cout << MasterDoesntExist;
        return;
    }
    int s_pos = getSlavePosById(m_id, s_id);
    if (s_pos == -1) {
        cout << SlaveDoesntExist;
        return;
    }
    char newCharValue[kGSStringMaxSize];
    strcpy(newCharValue, value.c_str());
    slaveFile.seekp(s_pos + 3 * sizeof(int));
    slaveFile.write(newCharValue, sizeof(newCharValue));
    cout << "Successfully updated\n";
}

void DataBase::updateSlaveGenre(int m_id, int s_id, const std::string &value) {
    if (!idToPos.count(m_id)) {
        cout << MasterDoesntExist;
        return;
    }
    int s_pos = getSlavePosById(m_id, s_id);
    if (s_pos == -1) {
        cout << SlaveDoesntExist;
        return;
    }
    char newCharValue[kGSStringMaxSize];
    strcpy(newCharValue, value.c_str());
    slaveFile.seekp(s_pos + 3 * sizeof(int) + kGameStringMaxSize);
    slaveFile.write(newCharValue, sizeof(newCharValue));
    cout << "Successfully updated\n";
}

void DataBase::cleanMasterFl() {
    //TODO
}

void DataBase::cleanSlaveFl() {
    //TODO
}


//
// Created by solon on 31.01.2021.
//

#ifndef DB_DATABASE_H
#define DB_DATABASE_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>

#include "GameStudio.h"
#include "Game.h"

enum class DBCommandType {
    GET_M,
    GET_S,
    DEL_M,
    DEL_S,
    UPDATE_M,
    UPDATE_S,
    INSERT_M,
    INSERT_S,
    CALC_M,
    CALC_S,
    UT_M,
    UT_S,
    EXIT,
    UNDEFINED
};

class DataBase {
public:
    DataBase();

    DBCommandType ProcessCommand(const std::string &command);

    ~DataBase();

private:
    std::fstream masterIndex;
    std::fstream masterFile;
    std::fstream slaveFile;
    std::ofstream log;
    std::map<int, int> idToPos;
    std::queue<int> deletedMasters;
    std::queue<int> deletedSlaves;

    void GetM(const std::string &command);

    void GetS(const std::string &command);

    void DelM(const std::string &command);

    void DelS(const std::string &command);

    void UpdateM(const std::string &command);

    void UpdateS(const std::string &command);

    void InsertM(const std::string &command);

    void InsertS(const std::string &command);

    void CalcM();

    void CalcS();

    void UtM();

    void UtS();

    void InsertM(int id, const std::string &name, const std::string &location);

    void InsertS(int m_id, int s_id, const std::string &name, const std::string &genre);

    void GetM(int id);

    void GetS(int m_id, int s_id);

    void DelM(int id);

    void DelS(int m_id, int s_id);

    void updateMasterName(int id, const std::string &value);

    void updateMasterLocation(int id, const std::string &value);

    void updateSlaveName(int m_id, int s_id, const std::string &value);

    void updateSlaveGenre(int m_id, int s_id, const std::string &value);;

    bool isSlaveAlreadyExist(int slave_iterator, int slave_num, int s_id);

    int getSlavePosById(int m_id, int s_id); // -1 => slave doesn`t exist
    void decrementSlave(int m_id);

    void rewriteIndex();

    void cleanMasterFl();

    void cleanSlaveFl();
};


#endif //DB_DATABASE_H

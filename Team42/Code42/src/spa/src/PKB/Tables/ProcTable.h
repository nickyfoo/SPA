#pragma once

#include <set>
#include <string>
#include <vector>
#include "Entities/Procedure.h"
#include <map>

class ProcTable {
public:
    ProcTable();
    ~ProcTable();

    int addProcedure(std::string name);
    int getNumProcedures();
    std::vector<Procedure*> getAllProcedures();
    void printProcs();

private:
    std::vector<Procedure*> table;
    std::map<std::string, int> procedureNameToIndex;
};
#pragma once

#include <set>
#include <string>
#include <vector>
#include "Entities/Procedure.h"
#include <map>

class ProcTable {

private: 
	static std::vector<Procedure*> table;
	static std::map<std::string, int> procedureNameToIndex;

public:
	static int addProcedure(std::string name);
	static int getNumProcedures();
	static std::vector<Procedure*> getAllProcedures();
	static void printProcs();
};
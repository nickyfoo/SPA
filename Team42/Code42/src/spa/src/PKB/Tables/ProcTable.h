#pragma once

#include <set>
#include <string>
#include <vector>
#include "Entities/Procedure.h"
#include <map>

using namespace std;

class ProcTable {

private: 
	static std::vector<Procedure> table;
	static std::map<std::string, int> procedureNameToIndex;

public:
	static int addProcedure(string name);
	static int getNumProcedures();
	static void printProcs();
};
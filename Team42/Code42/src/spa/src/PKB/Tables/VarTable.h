#pragma once
#include "../Entities/Variable.h"
#include <map>
#include <set>
#include <vector>

class VarTable {
private:
	static std::vector<Variable> table;
	static std::map<std::string, int> variableNameToIndex;

public:
	static int addVariable(std::string name);
	static int getNumVariables();
	static void printVars();
};
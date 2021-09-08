#pragma once
#include "../Entities/Variable.h"
#include <map>
#include <set>
#include <vector>

class VarTable {
public:
    VarTable();
    ~VarTable();

	int addVariable(std::string name);
	int getNumVariables();
	void printVars();
	std::vector<Variable*> getAllVariables();

private:
    std::vector<Variable*> table;
    std::map<std::string, int> variableNameToIndex;
};
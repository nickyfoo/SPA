#pragma once
#include "Variable.h";
#include <map>
#include <set>
#include <vector>

class VariableTable {
private:
	static set<string> uniqueVariables;
	static vector<Variable> table;
	static map<string, int> variableNameToIndex;
public:
	static int addVariable(string name);
};
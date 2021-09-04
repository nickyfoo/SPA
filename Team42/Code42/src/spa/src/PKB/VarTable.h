#pragma once
#include "Variable.h";
#include <map>
#include <set>
#include <vector>

class VarTable {
private:
	static vector<Variable> table;
	static map<string, int> variableNameToIndex;
public:
	static int addVariable(string name);
	static int getNumVariables();
	static void printVariables();
};
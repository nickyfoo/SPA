#include "VariableTable.h"
#include <iostream>

set<string> VariableTable::uniqueVariables;
vector<Variable> VariableTable::table;
map<string, int> VariableTable::variableNameToIndex;

int VariableTable::addVariable(string name) {

	map<string,int>::iterator it = variableNameToIndex.find(name);
	if(it!=variableNameToIndex.end()){
		return it->second;
	}
	else {
		int pos = table.size();
		table.push_back(Variable(pos, name));
		variableNameToIndex[name] = pos;
		return pos;
	}
};
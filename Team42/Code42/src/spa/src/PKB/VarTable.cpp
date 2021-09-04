#include "VarTable.h"
#include <iostream>

vector<Variable> VarTable::table;
map<string, int> VarTable::variableNameToIndex;

int VarTable::addVariable(string name) {
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


int VarTable::getNumVariables() {
	return variableNameToIndex.size();
}


void VarTable::printVariables() {
	for (Variable& v : table) {
		cout << v.getName() << "\n";
	}
}
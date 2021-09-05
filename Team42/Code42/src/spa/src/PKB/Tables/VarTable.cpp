#include "VarTable.h"
#include <iostream>

std::vector<Variable> VarTable::table;
std::map<std::string, int> VarTable::variableNameToIndex;

int VarTable::addVariable(std::string name) {
	std::map<std::string,int>::iterator it = variableNameToIndex.find(name);
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

void VarTable::printVars() {
	std::cout << "VarTable size: " << variableNameToIndex.size() << '\n';
	for (Variable& v : table) {
		std::cout << v.getName() << "\n";
	}
}
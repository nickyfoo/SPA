#include "VarTable.h"
#include <iostream>

VarTable::VarTable() {}

VarTable::~VarTable() {}

int VarTable::addVariable(std::string name) {
	std::map<std::string,int>::iterator it = variableNameToIndex.find(name);
	if(it!=variableNameToIndex.end()){
		return it->second;
	}
	else {
		int pos = table.size();
		Variable* v = new Variable(pos, name);
		table.push_back(v);
		variableNameToIndex[name] = pos;
		return pos;
	}
};

int VarTable::getNumVariables() {
	return variableNameToIndex.size();
}

std::vector<Variable*> VarTable::getAllVariables() {
    return table;
}

void VarTable::printVars() {
	std::cout << "VarTable size: " << variableNameToIndex.size() << '\n';
	for (Variable* v : table) {
		std::cout << v->getName() << "\n";
	}
}
#include "ProcTable.h"
#include <iostream>

vector<Procedure> ProcTable::table;
map<string, int> ProcTable::procedureNameToIndex;

int ProcTable::addProcedure(std::string name) {
	map<string, int>::iterator it = procedureNameToIndex.find(name);
	if (it != procedureNameToIndex.end()) {
		return it->second;
	}
	else {
		std::cout << "procedure: " << name << " added\n";
		int pos = table.size();
		table.push_back(Procedure(name));
		for (auto& x : table) {
			cout << x._name << '\n';
		}
		procedureNameToIndex[name] = pos;
		return pos;
	}
}


int ProcTable::getNumProcedures() {
	return procedureNameToIndex.size();
}

void ProcTable::printProcedures() {
	for (Procedure&p: table) {
		std::cout << p.getName() << '\n';
	}
}
#include "ProcTable.h"
#include <iostream>

std::vector<Procedure> ProcTable::table;
std::map<std::string, int> ProcTable::procedureNameToIndex;

int ProcTable::addProcedure(std::string name) {
	std::map<std::string, int>::iterator it = procedureNameToIndex.find(name);
	if (it != procedureNameToIndex.end()) {
		return it->second;
	} else {
		int pos = table.size();
		table.push_back(Procedure(name));
		procedureNameToIndex[name] = pos;
		return pos;
	}
}


int ProcTable::getNumProcedures() {
	return procedureNameToIndex.size();
}

void ProcTable::printProcs() {
	std::cout << "ProcTable size: " << table.size() << '\n';
	for (Procedure&p: table) {
		std::cout << p.getName() << '\n';
	}
}
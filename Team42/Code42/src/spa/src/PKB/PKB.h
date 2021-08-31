#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "UtilityFunctions.hpp";

using namespace std;

class PKB {
private:
	ast::Node* root;
	VariableTable variableTable;
	vector<void (*)(ast::Node* currentNode)> functions;
public:
	PKB(ast::Node* programRoot) {
		root = programRoot;
	}
	void initialPass(); 

};
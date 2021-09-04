#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "UtilityFunctions.hpp"
#include "FollowsFunctions.hpp"

using namespace std;

class PKB {
private:
	ast::Node* root;
public:
	PKB(ast::Node* programRoot) {
		root = programRoot;
	}
	void initialPass(); 
	void getFollows();


	// API called from here
	/*
	//todo: add issue about enums/discuss with people
	vector<Statement*> getStatements(ast::kind type);
	Statement* getStatementbyLineNo(lineNo);

	*/
};
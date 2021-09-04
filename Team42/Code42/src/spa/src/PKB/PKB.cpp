#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "PKB.h"
#include "../parser/AST.hpp"

using namespace std;

// Add variables, add statements
void PKB::initialPass() {
	vector<vector<void (*)(ast::Node* currentNode)>> functions(ast::NUM_KIND);
	
	functions[ast::Identifier].push_back(&UtilityFunctions::addVariable);
	for (int i = 0; i < ast::NUM_KIND; i++){
		if (Statement::isStmtKind((ast::Kind) i)) {
			functions[i].push_back(&UtilityFunctions::addStmt);
		}
	}
	functions[ast::Assign].push_back(&UtilityFunctions::addExprString);
	functions[ast::Procedure].push_back(&UtilityFunctions::addProc);
	ast::visit(root, functions);
	cout << "StmtList size: " << StmtTable::getNumStmts() << '\n';
	cout << "ProcTable size: " << ProcTable::getNumProcedures() << '\n';
	ProcTable::printProcedures();
	cout << "VarTable size: " << VarTable::getNumVariables() << '\n';
	VarTable::printVariables();
}

void PKB::getFollows() {
	vector<vector<void (*)(ast::Node* currentNode)>> functions(ast::NUM_KIND);
	functions[ast::Procedure].push_back(&FollowsFunctions::processProcedureNode);
	functions[ast::If].push_back(&FollowsFunctions::processIfNode);
	functions[ast::While].push_back(&FollowsFunctions::processWhileNode);
	ast::visit(root, functions);
	StmtTable::processFollows();
	StmtTable::processFollowsStar();
}


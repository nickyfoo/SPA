#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "PKB.h"
#include "../parser/AST.hpp"

using namespace std;

// Add variables, add statements
void PKB::initialPass() {
	vector<void (*)(ast::Node* currentNode)> functions(ast::NUM_KIND, &UtilityFunctions::defaultFunction);
	
	functions[ast::Identifier] = &UtilityFunctions::addVariable;
	for (int i = 0; i < ast::NUM_KIND; i++){
		if (Statement::isStmtKind((ast::Kind) i)) {
			functions[i] = &UtilityFunctions::addStmt;
		}

	}
	functions[ast::Assign] = &UtilityFunctions::addStmt;
	functions[ast::If] = &UtilityFunctions::addStmt;
	functions[ast::While] = &UtilityFunctions::addStmt;
	functions[ast::Read] = &UtilityFunctions::addStmt;
	functions[ast::Print] = &UtilityFunctions::addStmt;
	functions[ast::Call] = &UtilityFunctions::addStmt;
	functions[ast::Procedure] = &UtilityFunctions::addProc;
	ast::visit(root, functions);
	cout << "StmtList size: " << StmtTable::getNumStmts() << '\n';
	cout << "ProcTable size: " << ProcTable::getNumProcedures() << '\n';
	ProcTable::printProcedures();
	cout << "VarTable size: " << VarTable::getNumVariables() << '\n';
	VarTable::printVariables();
}

void PKB::getFollows() {
	vector<void (*)(ast::Node* currentNode)> functions(ast::NUM_KIND, &UtilityFunctions::defaultFunction);
	functions[ast::Procedure] = &FollowsFunctions::processProcedureNode;
	functions[ast::If] = &FollowsFunctions::processIfNode;
	functions[ast::While] = &FollowsFunctions::processWhileNode;
	ast::visit(root, functions);
	StmtTable::processFollows();
	StmtTable::processFollowsStar();
}


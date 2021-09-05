#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "PKB.h"
#include "../parser/AST.hpp"

// Add variables, add statements
void PKB::initialPass() {
	std::vector<std::vector<void (*)(ast::Node* currentNode)>> functions(ast::NUM_KIND);
	
	functions[ast::Identifier].push_back(&UtilityFunctions::addVariable);
	for (int i = 0; i < ast::NUM_KIND; i++){
		if (Statement::isStmtKind((ast::Kind) i)) {
			functions[i].push_back(&UtilityFunctions::addStmt);
		}
	}
	functions[ast::Assign].push_back(&UtilityFunctions::addExprString);
	functions[ast::Procedure].push_back(&UtilityFunctions::addProc);
	ast::visit(root, functions);

	StmtTable::printStmts();
	ProcTable::printProcs();
	VarTable::printVars();
}

void PKB::getFollows() {
	std::vector<std::vector<void (*)(ast::Node* currentNode)>> functions(ast::NUM_KIND);
	functions[ast::Procedure].push_back(&FollowsFunctions::processProcedureNode);
	functions[ast::If].push_back(&FollowsFunctions::processIfNode);
	functions[ast::While].push_back(&FollowsFunctions::processWhileNode);
	ast::visit(root, functions);
	StmtTable::processFollows();
	StmtTable::processFollowsStar();
}


void PKB::getParent() {
	std::vector<std::vector<void (*)(ast::Node* currentNode)>> functions(ast::NUM_KIND);
	functions[ast::If].push_back(&ParentFunctions::processIfNode);
	functions[ast::While].push_back(&ParentFunctions::processWhileNode);
	ast::visit(root, functions);
	StmtTable::processParent();
	StmtTable::processParentStar();
}

std::vector<Statement*> PKB::getStatements(ast::Kind type) {
    return StmtTable::getStatements(type);
}

std::vector<Statement*> PKB::getAllStatements() {
    return StmtTable::getAllStatements();
}
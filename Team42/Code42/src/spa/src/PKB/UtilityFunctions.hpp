#pragma once
#include "../parser/AST.hpp"
#include "VarTable.h"
#include "Statement.h"
#include "ProcTable.h"
#include "StmtTable.h"

class UtilityFunctions {
public:
	static void defaultFunction(ast::Node* node) {
		return;
	}

	static void addVariable(ast::Node* node) {
		ast::IdentifierNode* castedIdentifierNode = (ast::IdentifierNode*)node;
		VarTable::addVariable(castedIdentifierNode->name);
		return;
	}

	// Collect statements from procedure, if and while stmtList, 
	static void addStmt(ast::Node* node) {
		StmtTable::addStmt(node->lineNo, node->kind);
	}

	static void addProc(ast::Node* node) {
		ast::ProcedureNode* castedProcedureNode = (ast::ProcedureNode*) node;
		//ProcTable::addProcedure(castedProcedureNode->name) todo: ProcedureNode has no name field
		ProcTable::addProcedure("Procedure node of kind: " + to_string(castedProcedureNode->kind) + " lineno: "+ to_string(node->lineNo) + " colno: " + to_string(node->colNo));
	}
};
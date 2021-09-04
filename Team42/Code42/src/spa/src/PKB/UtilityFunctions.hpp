#pragma once
#include "../parser/AST.hpp"
#include "Tables/VarTable.h"
#include "Entities/Statement.h"
#include "Tables/ProcTable.h"
#include "Tables/StmtTable.h"

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

	static void addStmt(ast::Node* node) {
		StmtTable::addStmt(node);
	}

	static void addProc(ast::Node* node) {
		ast::ProcedureNode* castedProcedureNode = (ast::ProcedureNode*) node;
		//ProcTable::addProcedure(castedProcedureNode->name) todo: ProcedureNode has no name field
		ProcTable::addProcedure("Procedure node of kind: " + to_string(castedProcedureNode->kind) + " lineno: "+ to_string(node->lineNo) + " colno: " + to_string(node->colNo));
	}
};
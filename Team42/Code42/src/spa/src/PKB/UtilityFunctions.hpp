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

	static void addExprString(ast::Node* node) {
		if (node->kind == ast::Assign) {
			ast::AssignNode* castedAssignNode{ static_cast<ast::AssignNode*>(node) };
			StmtTable::getStmt(castedAssignNode->stmtNo)->setExprString(castedAssignNode->expr->exprString);
		}
	}

	static void addProc(ast::Node* node) {
		ast::ProcedureNode* castedProcedureNode = (ast::ProcedureNode*) node;
		ProcTable::addProcedure(castedProcedureNode->name);
	}
};
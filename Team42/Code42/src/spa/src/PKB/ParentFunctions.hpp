#pragma once
#include "../parser/AST.hpp"
#include "Tables/VarTable.h"
#include "Entities/Statement.h"
#include "Tables/ProcTable.h"
#include "Tables/StmtTable.h"
#include <algorithm>

class ParentFunctions {
public:
	static void processIfNode(ast::Node* node) {
		ast::IfNode* castedIfNode = (ast::IfNode*)node;
		Statement* ifStatement = StmtTable::getStatementByLineNo(castedIfNode->stmtNo);
		for (ast::Node* n : castedIfNode->thenStmtLst) {
			ifStatement->addChild(Statement::getStmtNo(n));
			StmtTable::getStatementByLineNo(Statement::getStmtNo(n))->addParent(ifStatement->getStmtNo()); //might want to do error checking here if NULL
		}

		for (ast::Node* n : castedIfNode->thenStmtLst) {
			ifStatement->addChild(Statement::getStmtNo(n));
			StmtTable::getStatementByLineNo(Statement::getStmtNo(n))->addParent(ifStatement->getStmtNo()); //might want to do error checking here if NULL
		}
	}

	static void processWhileNode(ast::Node* node) {
		ast::WhileNode* castedWhileNode = (ast::WhileNode*)node;
		Statement* whileStatement = StmtTable::getStatementByLineNo(castedWhileNode->stmtNo);
		for (ast::Node* n : castedWhileNode->stmtLst) {
			whileStatement->addChild(Statement::getStmtNo(n));
			StmtTable::getStatementByLineNo(Statement::getStmtNo(n))->addParent(whileStatement->getStmtNo()); //might want to do error checking here if NULL
		}
	}
};
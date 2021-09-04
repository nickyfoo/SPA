#pragma once
#include "../parser/AST.hpp"
#include "Tables/VarTable.h"
#include "Entities/Statement.h"
#include "Tables/ProcTable.h"
#include "Tables/StmtTable.h"
#include <algorithm>

class FollowsFunctions {
public:
	static void processProcedureNode(ast::Node* node) {
		ast::ProcedureNode* castedProcedureNode = (ast::ProcedureNode*)node;
		std::vector<int> lineNumbers;

		for (ast::Node* n : castedProcedureNode->stmtLst) {
			lineNumbers.push_back(Statement::getStmtNo(n));
		}
		sort(lineNumbers.begin(), lineNumbers.end());
		for (int i = 1; i < lineNumbers.size(); i++) {
			StmtTable::getStatementByLineNo(lineNumbers[i - 1])->addFollower(lineNumbers[i]);
			StmtTable::getStatementByLineNo(lineNumbers[i])->addFollowee(lineNumbers[i - 1]);
		}
	}
	
	static void processIfNode(ast::Node* node) {
		ast::IfNode* castedIfNode = (ast::IfNode*)node;
		std::vector<int> thenLineNumbers, elseLineNumbers;

		for (ast::Node* n : castedIfNode->thenStmtLst) {
			thenLineNumbers.push_back(Statement::getStmtNo(n));
		}
		sort(thenLineNumbers.begin(), thenLineNumbers.end());
		for (int i = 1; i < thenLineNumbers.size(); i++) {
			StmtTable::getStatementByLineNo(thenLineNumbers[i - 1])->addFollower(thenLineNumbers[i]);
			StmtTable::getStatementByLineNo(thenLineNumbers[i])->addFollowee(thenLineNumbers[i - 1]);
		}

		for (ast::Node* n : castedIfNode->thenStmtLst) {
			elseLineNumbers.push_back(Statement::getStmtNo(n));
		}
		sort(elseLineNumbers.begin(), elseLineNumbers.end());
		for (int i = 1; i < elseLineNumbers.size(); i++) {
			StmtTable::getStatementByLineNo(elseLineNumbers[i - 1])->addFollower(elseLineNumbers[i]);
			StmtTable::getStatementByLineNo(elseLineNumbers[i])->addFollowee(elseLineNumbers[i - 1]);
		}

	}

	static void processWhileNode(ast::Node* node) {
		ast::WhileNode* castedWhileNode = (ast::WhileNode*)node;
		//todo: not sure how the stmtlst is organized/sorted, so I'll store line numbers for now and sort them
		std::vector<int> lineNumbers;
		for (ast::Node* n : castedWhileNode->stmtLst) {
			lineNumbers.push_back(Statement::getStmtNo(n));
		}
		sort(lineNumbers.begin(), lineNumbers.end());
		for (int i = 1; i < lineNumbers.size(); i++) {
			StmtTable::getStatementByLineNo(lineNumbers[i - 1])->addFollower(lineNumbers[i]);
			StmtTable::getStatementByLineNo(lineNumbers[i])->addFollowee(lineNumbers[i - 1]);
		}
	}
};
#pragma once
#include "../parser/AST.hpp"
#include "VarTable.h"
#include "Statement.h"
#include "ProcTable.h"
#include "StmtTable.h"
#include <algorithm>

class ParentFunctions {
public:
	static void processIfNode(ast::Node* node) {
		ast::IfNode* castedIfNode = (ast::IfNode*)node;
		//todo: not sure how the stmtlst is organized/sorted, so I'll store line numbers for now and sort them
		vector<int> thenLineNumbers, elseLineNumbers;
		//todo: do this in a better way
		for (ast::Node* n : castedIfNode->thenStmtLst) {
			thenLineNumbers.push_back(n->lineNo);
		}
		sort(thenLineNumbers.begin(), thenLineNumbers.end());
		for (int i = 1; i < thenLineNumbers.size(); i++) {
			StmtTable::getStmt(thenLineNumbers[i - 1])->addFollower(thenLineNumbers[i]);
			StmtTable::getStmt(thenLineNumbers[i])->addFollowee(thenLineNumbers[i - 1]);
		}

		for (ast::Node* n : castedIfNode->thenStmtLst) {
			elseLineNumbers.push_back(n->lineNo);
		}
		sort(elseLineNumbers.begin(), elseLineNumbers.end());
		for (int i = 1; i < elseLineNumbers.size(); i++) {
			StmtTable::getStmt(elseLineNumbers[i - 1])->addFollower(elseLineNumbers[i]);
			StmtTable::getStmt(elseLineNumbers[i])->addFollowee(elseLineNumbers[i - 1]);
		}

	}

	static void processWhileNode(ast::Node* node) {
		ast::WhileNode* castedWhileNode = (ast::WhileNode*)node;
		//todo: not sure how the stmtlst is organized/sorted, so I'll store line numbers for now and sort them
		vector<int> lineNumbers;
		for (ast::Node* n : castedWhileNode->stmtLst) {
			lineNumbers.push_back(n->lineNo);
		}
		sort(lineNumbers.begin(), lineNumbers.end());
		for (int i = 1; i < lineNumbers.size(); i++) {
			StmtTable::getStmt(lineNumbers[i - 1])->addFollower(lineNumbers[i]);
			StmtTable::getStmt(lineNumbers[i])->addFollowee(lineNumbers[i - 1]);
		}
	}
};
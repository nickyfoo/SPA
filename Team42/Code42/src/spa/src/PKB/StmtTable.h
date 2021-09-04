#pragma once
#include "Statement.h"
#include <map>
#include <set>
#include <vector>


class StmtTable {
private:
	static std::map<int,Statement> table;
	static std::set<std::pair<int, int>> Follows, Follows_star;
	static std::set<int> WhileStatements, IfStatements, AssignmentStatements;
	static int largestStmtNum;

public:
	static void addStmt(int lineNo, ast::Kind kind);
	static Statement* getStmt(int lineNo);
	static int getNumStmts();
	static int getLargestStmtNum();
	
	static void processFollows();
	static void processFollowsStar();

	static void printStmts();

		//todo: 
	//vector<Statement*> getWhile
};
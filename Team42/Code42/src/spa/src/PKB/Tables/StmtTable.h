#pragma once
#include "Entities/Statement.h"
#include <map>
#include <set>
#include <vector>


class StmtTable {
private:
	static std::map<int,Statement> table;
	static std::set<std::pair<int, int>> Follows, Follows_star;
	static std::map<ast::Kind, std::vector<Statement*>> typeToStatement;
	static int largestStmtNum;
public:
	static void addStmt(ast::Node* node);

	static Statement* getStmt(int lineNo);
	
	static int getNumStmts();
	
	static void processFollows();
	static void processFollowsStar();

	static void printStmts();

	static int getLargestStmtNum();

		//todo: 
	//vector<Statement*> getWhile

	/*
		Statement s;
		Select s;

		return a ptr to vector<Statment*>

	*/
};
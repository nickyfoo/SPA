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
	//largestStmtNum is used to compute the transitive closure
	static int largestStmtNum;
public:
	static void addStmt(ast::Node* node);

	
	static void processFollows();

	static int getLargestStmtNum();
	static void processFollowsStar();

	//API calls for PQL
	static std::vector<Statement*> getStatements(ast::Kind type);
	static Statement* getStatementByLineNo(int lineNo);

	//Debug functions
	static void printStmts();
	void printStmtInfos();
};
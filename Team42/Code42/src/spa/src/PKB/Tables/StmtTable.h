#pragma once
#include "Entities/Statement.h"
#include <map>
#include <set>
#include <vector>


class StmtTable {
private:
    // Largest number of statements in the graph
    static int largestStmtNum;
	static std::map<int,Statement*> table;
	static std::vector<Statement*> allStatements;
	static std::map<ast::Kind, std::vector<Statement*>> typeToStatement;
	static std::set<std::pair<int, int>> Follows, Follows_star;
	static std::set<std::pair<int, int>> Parent, Parent_star;

public:
	static void addStmt(ast::Node* node);

	static int getLargestStmtNum();
	static void processFollows();
	static void processFollowsStar();
	static void processParent();
	static void processParentStar();

	// API calls for PQL
	static std::vector<Statement*> getStatements(ast::Kind type);
	static Statement* getStatementByLineNo(int lineNo);
	static std::vector<Statement*> getAllStatements();

	// Debug functions
	static void printStmts();
	void printStmtInfos();
};
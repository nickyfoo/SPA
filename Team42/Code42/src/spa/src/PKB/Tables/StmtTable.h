#pragma once
#include "Entities/Statement.h"
#include <map>
#include <set>
#include <vector>


class StmtTable {
public:
    StmtTable();
    ~StmtTable();

	void addStmt(ast::Node* node);

	int getLargestStmtNum();
	void processFollows();
	void processFollowsStar();
	void processParent();
	void processParentStar();

	// API calls for PQL
	std::vector<Statement*> getStatements(ast::Kind type);
	Statement* getStatementByLineNo(int lineNo);
	std::vector<Statement*> getAllStatements();

	// Debug functions
	void printStmts();
	void printStmtInfos();

private:
    // Largest number of statements in the graph
    int largestStmtNum;
    std::map<int, Statement> table;
    std::vector<Statement*> allStatements;
    std::map<ast::Kind, std::vector<Statement*>> typeToStatement;
    std::set<std::pair<int, int>> Follows, Follows_star;
    std::set<std::pair<int, int>> Parent, Parent_star;
};
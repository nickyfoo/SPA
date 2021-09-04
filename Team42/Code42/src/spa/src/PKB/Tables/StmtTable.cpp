#include "StmtTable.h"
#include <iostream>
#include "TransitiveClosure.h"

std::map<int, Statement> StmtTable::table;
int StmtTable::largestStmtNum = 0;
std::set<std::pair<int, int>> StmtTable::Follows, StmtTable::Follows_star;
std::map<ast::Kind, std::vector<Statement*>> StmtTable::typeToStatement;

void StmtTable::addStmt(ast::Node* node){
	int stmtNo = Statement::getStmtNo(node);
	if (stmtNo == 0) return; // Not a statement node
	Statement s(stmtNo, node->kind);
	table[stmtNo] = s;
	typeToStatement[node->kind].push_back(&s);
	largestStmtNum = max(largestStmtNum, stmtNo);
};

//todo: might want to add a check for valid lineNo..
Statement* StmtTable::getStmt(int lineNo) {
	std::map<int, Statement>::iterator it = table.find(lineNo);
	if (it != table.end()) {
		return &(it->second);
	}
	else {
		//probably not a good idea to return NULL, but works for now.
		return NULL;
	}
}

int StmtTable::getNumStmts() {
	return table.size();
}

int StmtTable::getLargestStmtNum() {
	return largestStmtNum;
}

// Gets Follows relationship from Statements in preparation to get transitive closure.
void StmtTable::processFollows() {
	for (auto& [lineNo, stmt] : table) {
		for (auto& followerLineNo : *(stmt.getFollowers())) {
			Follows.insert({ lineNo,followerLineNo });
		}
	}
}

void StmtTable::processFollowsStar() {
	int n = largestStmtNum + 1;
	std::vector<std::vector<int>> d = TransitiveClosure::getTransitiveClosure(Follows,n);
	for (int i = 0; i < n; i++) {
		Statement* stmt = getStmt(i);
		if (stmt != NULL) {
			for (int j = 0; j < n; j++) {
				if (d[i][j] != TransitiveClosure::INF) {
					stmt->addFollowerStar(j);
					getStmt(j)->addFolloweeStar(i);
				}
			}
		}
	}
}

void StmtTable::printStmts() {
	std::cout << "StmtTable size: " << table.size() << '\n';
	for (auto&[k,x] : table) {
		std::cout << k << ' ' << x.getKind() << ' ';
		if (x.getKind() == ast::Assign) std::cout << x.getExprString();
		std::cout << '\n';
	}
}

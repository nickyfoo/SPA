#include "StmtTable.h"
#include <iostream>
#include "TransitiveClosure.h"

StmtTable::StmtTable() {
    largestStmtNum = 0;
}

StmtTable::~StmtTable() = default;

void StmtTable::addStmt(ast::Node* node) {
	int stmtNo = Statement::getStmtNo(node);
	if (stmtNo == 0) return; // Not a statement node
	Statement s(stmtNo, node->kind);
	table[stmtNo] = s;
	typeToStatement[node->kind].push_back(&table[stmtNo]);
	allStatements.push_back(&table[stmtNo]);
	largestStmtNum = std::max(largestStmtNum, stmtNo);
}

//todo: might want to add a check for valid lineNo..
Statement* StmtTable::getStatementByLineNo(int lineNo) {
	auto it = table.find(lineNo);
	if (it != table.end()) {
		return &(it->second);
	}
	else {
		//probably not a good idea to return NULL, but works for now.
		return nullptr;
	}
}

int StmtTable::getLargestStmtNum() {
	return largestStmtNum;
}

// Gets Follow relationship from Statements in preparation to get transitive closure.
void StmtTable::processFollows() {
	for (auto& [lineNo, stmt] : table) {
		for (auto& followerLineNo : *(stmt.getFollowers())) {
			Follows.insert({ lineNo, followerLineNo });
		}
	}
}

void StmtTable::processFollowsStar() {
	int n = largestStmtNum + 1;
	std::vector<std::vector<int>> d = TransitiveClosure::getTransitiveClosure(Follows,n);
	for (int i = 0; i < n; i++) {
		Statement* stmt = getStatementByLineNo(i);
		if (stmt != nullptr) {
			for (int j = 0; j < n; j++) {
				if (d[i][j] != TransitiveClosure::INF) {
					stmt->addFollowerStar(j);
					getStatementByLineNo(j)->addFolloweeStar(i);
				}
			}
		}
	}
}

// Gets Follow relationship from Statements in preparation to get transitive closure.
void StmtTable::processParent() {
	for (auto& [lineNo, stmt] : table) {
		for (auto& childLineNo : *(stmt.getChildren())) {
			Parent.insert({ lineNo,childLineNo });
		}
	}
}

void StmtTable::processParentStar() {
	int n = largestStmtNum + 1;
	std::vector<std::vector<int>> d = TransitiveClosure::getTransitiveClosure(Parent, n);
	for (int i = 0; i < n; i++) {
		Statement* stmt = getStatementByLineNo(i);
		if (stmt != nullptr) {
			for (int j = 0; j < n; j++) {
				if (d[i][j] != TransitiveClosure::INF) {
					stmt->addChildStar(j);
					getStatementByLineNo(j)->addParentStar(i);
				}
			}
		}
	}
}


void StmtTable::printStmts() {
	std::cout << "StmtTable size: " << table.size() << '\n';
	for (auto&[k,x] : table) {
		std::cout << k << ": " << x.getKind() << ' ';
		if (x.getKind() == ast::Assign) std::cout << x.getExprString();
		std::cout << '\n';
	}
}

void StmtTable::printStmtInfos() {
	for (auto& [k, x] : table) {
		x.FollowsInfo();
		x.ParentInfo();
	}
}

std::vector<Statement*> StmtTable::getStatements(ast::Kind type) {
	return typeToStatement[type];
}

std::vector<Statement*> StmtTable::getAllStatements() {
	return allStatements;
}
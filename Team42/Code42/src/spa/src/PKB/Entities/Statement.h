#pragma once
#include "AST.hpp"
#include <set>

class Statement {
public:
	Statement(int lineNo, ast::Kind type) {
		_lineNo = lineNo;
		_type = type;
	}
	
	Statement() {

	};

	int getLineNo();
	ast::Kind getType();
	//todo: refactor into void addLineNo(int lineNo, set<int> list)

	void addLineNo(int lineNo, std::set<int>& container);
	void addFollower(int lineNo);
	void addFollowerStar(int lineNo);
	void addFollowee(int lineNo);
	void addFolloweeStar(int lineNo);
	//void addParent(int lineNo);
	//void addParentStar(int lineNo);
	//void addChild(int lineNo);
	//void addChildStar(int lineNo);

	void printFollowers();

	void info();
	std::set<int>* getFollowers();

	static bool isStmt(ast::Node* node);
	static int getStmtNo(ast::Node* node);

private:
	int _lineNo;
	ast::Kind _type;
	// for v in Followers, Follows(this, v) is true.
	// for v in Followees, Follows(v, this) is true.
	std::set<int> Followers, FollowersStar, Followees, FolloweesStar;
	// for v in Parents, Parent(v, this) is true.
	// for v in Children, Parent(this, v) is true.
	std::set<int> Parents, ParentsStar, Children, ChildrenStar;
};
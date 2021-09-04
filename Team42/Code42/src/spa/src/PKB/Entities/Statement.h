#pragma once
#include "AST.hpp"
#include <set>

class Statement {
public:
	Statement(int lineNo, ast::Kind type) {
		_lineNo = lineNo;
		_kind = type;
	}
	
	Statement() {

	};

	int getLineNo();
	ast::Kind getKind();
	//todo: refactor into void addLineNo(int lineNo, set<int> list)


	void Statement::setExprString(std::string exprString);

	void addLineNo(int lineNo, std::set<int>& container);
	void addFollower(int lineNo);
	void addFollowerStar(int lineNo);
	void addFollowee(int lineNo);
	void addFolloweeStar(int lineNo);
	//void addParent(int lineNo);
	//void addParentStar(int lineNo);
	//void addChild(int lineNo);
	//void addChildStar(int lineNo);



	void info();

	std::set<int>* getFollowers();
	void printFollowers();

	static bool isStmtKind(ast::Kind kind);
	static bool isStmtNode(ast::Node* node);
	static int getStmtNo(ast::Node* node);
	std::string getExprString();



private:
	int _lineNo;
	std::string _exprString;
	ast::Kind _kind;
	// for v in Followers, Follows(this, v) is true.
	// for v in Followees, Follows(v, this) is true.
	std::set<int> Followers, FollowersStar, Followees, FolloweesStar;
	// for v in Parents, Parent(v, this) is true.
	// for v in Children, Parent(this, v) is true.
	std::set<int> Parents, ParentsStar, Children, ChildrenStar;
};
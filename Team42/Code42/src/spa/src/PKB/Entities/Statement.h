#pragma once
#include "AST.hpp"
#include <set>

class Statement {
public:
	Statement(int lineNo, ast::Kind type) {
		_stmtNo = lineNo;
		_kind = type;
	}
	
	Statement() {};

	//Getters
	int getStmtNo();
	ast::Kind getKind();
	//todo: Only applicable for AssignStatement for now, need to add if/while
	std::string getExprString(); 

	std::set<int>* getFollowers();
	std::set<int>* getChildren();


	//todo: refactor into void addLineNo(int lineNo, set<int> list)


	void setExprString(std::string exprString);

	void addFollower(int lineNo);
	void addFollowerStar(int lineNo);
	void addFollowee(int lineNo);
	void addFolloweeStar(int lineNo);
	void addParent(int lineNo);
	void addParentStar(int lineNo);
	void addChild(int lineNo);
	void addChildStar(int lineNo);

	//Debug functions
	void FollowsInfo();
	void ParentInfo();

	static bool isStmtKind(ast::Kind kind);
	static bool isStmtNode(ast::Node* node);
	static int getStmtNo(ast::Node* node);



private:
	int _stmtNo;
	std::string _exprString = "";
	ast::Kind _kind;

	// for v in Followers, Follows(this, v) is true.
	// for v in Followees, Follows(v, this) is true.
	std::set<int> Followers, FollowersStar, Followees, FolloweesStar;
	// for v in Parents, Parent(v, this) is true.
	// for v in Children, Parent(this, v) is true.
	std::set<int> Parents, ParentsStar, Children, ChildrenStar;
};
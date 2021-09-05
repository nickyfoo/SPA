#include "Statement.h"
#include <iostream>

int Statement::getStmtNo() {
	return _stmtNo;
}
ast::Kind Statement::getKind() {
	return _kind;
}

void Statement::addFollower(int lineNo) {
	Followers.insert(lineNo);
}
void Statement::addFollowerStar(int lineNo) {
	FollowersStar.insert(lineNo);
}
void Statement::addFollowee(int lineNo) {
	Followees.insert(lineNo);
}
void Statement::addFolloweeStar(int lineNo) {
	FolloweesStar.insert(lineNo);
}

void Statement::addParent(int lineNo) {
	Parents.insert(lineNo);
}
void Statement::addParentStar(int lineNo) {
	ParentsStar.insert(lineNo);
}

void Statement::addChild(int lineNo) {
	Children.insert(lineNo);
}
void Statement::addChildStar(int lineNo) {
	ChildrenStar.insert(lineNo);
}

void Statement::setExprString(std::string exprString) {
	_exprString = exprString;
}

std::string Statement::getExprString() {
	return _exprString;
}

void Statement::FollowsInfo() {
	std::cout << "Statement " << _stmtNo << " is followed by:\n";
	std::cout << "Followers: ";
	for (auto& x : Followers) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "FollowersStar: ";
	for (auto& x : FollowersStar) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "Statement " << _stmtNo << " is a followee of/follows:\n";
	std::cout << "Followees: ";
	for (auto& x : Followees) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "FolloweesStar: ";
	for (auto& x : FolloweesStar) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << '\n';
}
void Statement::ParentInfo() {
	std::cout << "Statement " << _stmtNo << " has parents:\n";
	std::cout << "Parents: ";
	for (auto& x : Parents) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "ParentsStar: ";
	for (auto& x : ParentsStar) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "Statement " << _stmtNo << " has children:\n";
	std::cout << "Children: ";
	for (auto& x : Children) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "ChildrenStar: ";
	for (auto& x : ChildrenStar) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << '\n';
}


std::set<int>* Statement::getFollowers() {
	return &Followers;
}

std::set<int>* Statement::getChildren() {
	return &Children;
}


bool Statement::isStmtKind(ast::Kind kind) {
	switch (kind) {
	case ast::Assign:
	case ast::If:
	case ast::While:
	case ast::Read:
	case ast::Print:
	case ast::Call:
		return true;
	default:
		return false;
	}
}

bool Statement::isStmtNode(ast::Node* node) {
	return isStmtKind(node->kind);
}

int Statement::getStmtNo(ast::Node* node) {
	switch (node->kind) {
	case ast::Assign: {
		ast::AssignNode* castedAssignNode{ static_cast<ast::AssignNode*>(node) };
		return castedAssignNode->stmtNo;
	}
	case ast::If: {
		ast::IfNode* castedIfNode{ static_cast<ast::IfNode*>(node) };
		return castedIfNode->stmtNo;
	}
	case ast::While: {
		ast::WhileNode* castedWhileNode{ static_cast<ast::WhileNode*>(node) };
		return castedWhileNode->stmtNo;
	}
	case ast::Read: {
		ast::ReadNode* castedReadNode{ static_cast<ast::ReadNode*>(node) };
		return castedReadNode->stmtNo;
	}
	case ast::Print: {
		ast::PrintNode* castedPrintNode{ static_cast<ast::PrintNode*>(node) };
		return castedPrintNode->stmtNo;
	}
	case ast::Call: {
		ast::CallNode* castedCallNode{ static_cast<ast::CallNode*>(node) };
		return castedCallNode->stmtNo;
	}
	default: {
		return 0;
	}
	}
};
#include "Statement.h"
#include <iostream>
int Statement::getLineNo() {
	return _lineNo;
}
ast::Kind Statement::getType() {
	return _type;
}

void Statement::addLineNo(int lineNo, std::set<int>&container) {
	container.insert(lineNo);
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


void Statement::printFollowers() {
	for (auto& x : Followers) {
		std::cout << x << '\n';
	}
}

void Statement::info() {
	std::cout << "Statement " << _lineNo << " is followed by:\n";
	std::cout << "Followers: ";
	for (auto& x : Followers) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "FollowersStar: ";
	for (auto& x : FollowersStar) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "Statement " << _lineNo << " is a followee of/follows:\n";
	std::cout << "Followees: ";
	for (auto& x : Followees) {
		std::cout << x << ' ';
	} std::cout << '\n';
	std::cout << "FolloweesStar: ";
	for (auto& x : FolloweesStar) {
		std::cout << x << ' ';
	} std::cout << '\n';
}


std::set<int>* Statement::getFollowers() {
	return &Followers;
}

bool Statement::isStmt(ast::Node* node) {
	switch (node->kind) {
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
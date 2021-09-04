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
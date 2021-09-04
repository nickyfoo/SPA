#pragma once
#include <string>
#include <set>

class Procedure {
public:
	Procedure(std::string name) {
		_name = name;
	}
	std::string getName();
	std::string _name;
private:
	std::set<int> Calls;
};
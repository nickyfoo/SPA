#pragma once
#include <string>

class Variable {
private:
	int _index;
	std::string _name;

public:
	Variable(int index, std::string name) {
		_index = index;
		_name = name;
	}
	std::string getName();
};
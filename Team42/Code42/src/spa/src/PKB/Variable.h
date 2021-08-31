#pragma once
#include <string>

using namespace std;

class Variable {
private:
	int _index;
	string _name;

public:
	Variable(int index, string name) {
		_index = index;
		_name = name;
	}
};
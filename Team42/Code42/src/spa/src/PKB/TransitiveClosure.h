#pragma once
#include <vector>
#include <set>

using namespace std;

class TransitiveClosure {
public:
	static const int INF = 1e9;
	//getting transitive closure from FloydWarshall
	static vector<vector<int>> getTransitiveClosure(set<pair<int, int>> al, int n);

};
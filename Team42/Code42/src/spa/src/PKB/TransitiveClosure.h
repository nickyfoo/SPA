#pragma once
#include <vector>
#include <set>

class TransitiveClosure {
 public:
  inline static const int INF = 1e9;
  //getting transitive closure from FloydWarshall
  static std::vector<std::vector<int>> getTransitiveClosure(std::set<std::pair<int, int>> al, int n);

};
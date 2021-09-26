#pragma once

#include <set>
#include <vector>
#include <algorithm>

#include "../entities/entity.hpp"

class Table {
 public:
  // Gets transitive closure by running a DFS from each node.
  std::vector<std::vector<int>> GetTransitiveClosure(std::map<int, std::vector<int>> al,
                                                            int n) {
    std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
      DFS(i, i, d, al);
    }
    return d;
  }

  void DFS(int start, int u, std::vector<std::vector<int>>& d, std::map<int, std::vector<int>>& al) {
    for (auto& v : al[u]) {
      if (d[start][v] == 0) {
        d[start][v] = 1;
        DFS(start, v, d, al);
      }
    }
  }
};

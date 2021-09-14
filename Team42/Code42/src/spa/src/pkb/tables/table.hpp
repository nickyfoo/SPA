#pragma once

#include <set>
#include <utility>
#include <vector>
#include <algorithm>

#include "../entities/entity.hpp"

class Table {
 public:
  // Infinite value.
  inline static const int kInf = 1e9;

  // Gets transitive closure using the Floyd Warshall algorithm.
  static std::vector<std::vector<int>> GetTransitiveClosure(std::set<std::pair<int, int>> al,
                                                            int n) {
    std::vector<std::vector<int>> d(n, std::vector<int>(n, kInf));
    for (auto&[u, v] : al) {
      d[u][v] = 1;
    }
    for (int k = 0; k < n; k++) {
      for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
          d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
        }
      }
    }
    return d;
  }
};

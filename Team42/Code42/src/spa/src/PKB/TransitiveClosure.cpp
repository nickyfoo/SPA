#include "TransitiveClosure.h"

// Getting transitive closure from Floyd Warshall
std::vector<std::vector<int>> TransitiveClosure::getTransitiveClosure(std::set<std::pair<int, int>> al, int n) {
	std::vector<std::vector<int>> d(n, std::vector<int>(n, INF));
	for (auto& [u, v] : al) {
		d[u][v] = 1;
	}
	for (int k = 0; k < n; k++) {
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < n; j++) {
				if (d[i][j] > d[i][k] + d[k][j]) {
					d[i][j] = d[i][k] + d[k][j];
				}
			}
		}
	}
	return d;
}
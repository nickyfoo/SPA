#include "clause_group.h"

#include <utility>
#include <algorithm>

ClauseGroup::ClauseGroup() = default;

ClauseGroup::~ClauseGroup() = default;

std::vector<ClauseVertex> ClauseGroup::get_clauses() {
  return this->clauses_;
}

void ClauseGroup::AddSynUsed(std::string syn) {
  this->synonyms_used_.push_back(syn);
}

std::vector<std::string> ClauseGroup::get_syn_used() {
  return this->synonyms_used_;
}

void ClauseGroup::AddClause(ClauseVertex clause_vertex) {
  this->clauses_.push_back(clause_vertex);
}

void ClauseGroup::set_clauses(std::vector<ClauseVertex> clauses) {
  this->clauses_ = clauses;
}

void ClauseGroup::SortWithinGroup() {
  // sort clauses by priority
  std::sort(this->clauses_.begin(),
            this->clauses_.end(),
            [](ClauseVertex &lhs, ClauseVertex &rhs) {
              return lhs.get_priority() < rhs.get_priority();
            });

  // use prim's algorithm to order clauses such that synonyms in a clause were evaluated in
  // a previous clause

  // Create adjacency list
  std::unordered_map<int, std::vector<std::pair<int, ClauseVertex>>> adj_list;
  // adding empty vector to each clause in map
  for (auto clause : clauses_) {
    std::vector<std::pair<int, ClauseVertex>> edges;
    adj_list[clause.get_id()] = edges;
  }

  for (int i = 0; i < clauses_.size(); i++) {
    for (int j = i + 1; j < clauses_.size(); j++) {
      auto first_clause = clauses_.at(i);
      auto second_clause = clauses_.at(j);
      for (auto syn : first_clause.get_synonyms_used()) {
        auto syns_used = second_clause.get_synonyms_used();
        if (std::find(syns_used.begin(),
                      syns_used.end(),
                      syn) != syns_used.end()) {
          // if clause at j has a common synonym as clause i
          // add to adj list for both sides
          adj_list[first_clause.get_id()].push_back(
              std::make_pair(second_clause.get_priority(), second_clause));
          adj_list[second_clause.get_id()].push_back(
              std::make_pair(first_clause.get_priority(), first_clause));
          break;
        }
      }
    }
  }

  // Creating visits set and priority queue
  std::unordered_set<int> visited;
  std::priority_queue<std::pair<int, ClauseVertex>,
                      std::vector<std::pair<int, ClauseVertex>>,
                      std::greater<>> pqueue;
  pqueue.push(std::make_pair(clauses_.at(0).get_priority(), clauses_.at(0)));

  // Looping through priority queue and finding MST
  std::vector<ClauseVertex> new_clauses;
  while (visited.size() < clauses_.size() && !pqueue.empty()) {
    // get first item from priority queue
    auto pair = pqueue.top();
    pqueue.pop();
    auto curr_vertex = pair.second;

    if (visited.find(curr_vertex.get_id()) != visited.end()) {
      // if has visited alr, continue
      continue;
    }
    // Add curr vertex to visited and add to clauses
    visited.insert(curr_vertex.get_id());
    new_clauses.push_back(curr_vertex);

    // Add edges to priority queue
    for (auto edges : adj_list[curr_vertex.get_id()]) {
      pqueue.push(edges);
    }
  }

  // Setting clauses
  this->clauses_ = new_clauses;
}

#include <algorithm>

#include "query_optimizer.h"

int count = 0;
QueryOptimizer::QueryOptimizer(std::vector<SuchThatClause *> *relationships,
                               std::vector<PatternClause *> *patterns,
                               std::vector<WithClause *> *withs,
                               std::vector<ResultClause *> *return_entities) {
  this->relationships_ = relationships;
  this->patterns_ = patterns;
  this->withs_ = withs;
  this->return_entities_ = return_entities;
}

std::vector<std::shared_ptr<ClauseGroup>> QueryOptimizer::CreateGroupings() {
  std::vector<std::shared_ptr<ClauseGroup>> clause_groupings;
  std::vector<ClauseVertex> no_syn_used;
  std::vector<ClauseVertex> has_syn_used;
  std::vector<ClauseVertex> has_return_syn_used;

  // Iterating through relationship, pattern and with vectors
  // and creating ClauseVertices.
  std::vector<ClauseVertex> all_vertices = CombineAllVertices(MakeSuchThatVertices(relationships_),
                                                              MakePatternVertices(patterns_),
                                                              MakeWithVertices(withs_));
  // Add to no_syn_used vector if clause vertex has no synonyms used.
  std::copy_if(all_vertices.begin(),
               all_vertices.end(),
               std::back_inserter(no_syn_used), [](auto vertex) { return vertex.get_synonyms_used().empty(); });
  // Add to has_syn_used if clause vertex uses synonyms.
  std::copy_if(all_vertices.begin(),
               all_vertices.end(),
               std::back_inserter(has_syn_used), [](auto vertex) { return !vertex.get_synonyms_used().empty(); });
  // Add to has_return_syn_used if clause vertex uses return synonyms.
  std::copy_if(all_vertices.begin(),
               all_vertices.end(),
               std::back_inserter(has_return_syn_used), [](auto vertex) { return vertex.get_has_return_syn(); });

  // Group with no synonyms will be evaluated first
  auto no_syn_group = ClauseGroup();
  no_syn_group.set_clauses(no_syn_used);
  no_syn_group.SortWithinGroup();
  std::shared_ptr<ClauseGroup> no_syn_group_ptr = std::make_shared<ClauseGroup>(no_syn_group);
  clause_groupings.push_back(no_syn_group_ptr);

  // Group rest of vertices based on connected synonyms
  // Create unordered_map mapping synonym to the clauseVertex it appears in.
  // Also create a has_visited_syn map to map each synonym to a boolean.
  std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause;
  std::unordered_map<std::string, std::vector<ClauseVertex>> return_syn_to_clause;
  auto *has_visited_syn = new std::unordered_map<std::string, bool>();
  auto *has_visited_clause = new std::unordered_map<int, bool>();
  std::vector<std::shared_ptr<ClauseGroup>> return_syn_clause_groupings;

  for (ClauseVertex clause_vertex : has_syn_used) {
    has_visited_clause->insert({clause_vertex.get_id(), false});
    for (std::string syn : clause_vertex.get_synonyms_used()) {
      if (syn_to_clause.find(syn) != syn_to_clause.end() &&
          clause_vertex.get_has_return_syn()) {
        // Add to both maps if clause vertex has return synonym.
        syn_to_clause[syn].push_back(clause_vertex);
        return_syn_to_clause[syn].push_back(clause_vertex);
      } else if (syn_to_clause.find(syn) != syn_to_clause.end()) {
        syn_to_clause[syn].push_back(clause_vertex);
      } else if (clause_vertex.get_has_return_syn()) {
        syn_to_clause.insert({syn, {clause_vertex}});
        return_syn_to_clause.insert({syn, {clause_vertex}});
        has_visited_syn->insert({syn, false});
      } else {
        syn_to_clause.insert({syn, {clause_vertex}});
        has_visited_syn->insert({syn, false});
      }
    }
  }

  // iterate through return synonyms and run dfs to group clauses based on connected synonyms.
  for (auto pair : return_syn_to_clause) {
    if (!has_visited_syn->at(pair.first)) {
      // run dfs if synonym has not been visited
      ClauseGroup *has_return_syn_group = new ClauseGroup();
      FindConnectedGroups(has_return_syn_group, syn_to_clause, has_visited_syn, has_visited_clause,
                          pair.first);
      for (std::string syn_used : has_return_syn_group->get_syn_used()) {
        // Remove synonyms used from syn_to_clause if they are connected to return syns
        syn_to_clause.erase(syn_to_clause.find(syn_used));
      }
      has_return_syn_group->SortWithinGroup();
      std::shared_ptr<ClauseGroup> connected_syn_group_ptr =
          std::make_shared<ClauseGroup>(*has_return_syn_group);
      return_syn_clause_groupings.push_back(connected_syn_group_ptr);
    }
  }

  // Create and add group with no return synonyms
  clause_groupings.push_back(MakeNoReturnSynGroup(syn_to_clause, has_visited_clause));

  // If there are no clauses with return synonym, add an empty group to the vector.
  if (has_return_syn_used.empty()) {
    auto has_return_syn_group = ClauseGroup();
    std::shared_ptr<ClauseGroup> has_return_syn_group_ptr =
        std::make_shared<ClauseGroup>(has_return_syn_group);
    clause_groupings.push_back(has_return_syn_group_ptr);
  } else {
    // Add rest of groups with connected return synonyms
    clause_groupings.insert(clause_groupings.end(), return_syn_clause_groupings.begin(),
                            return_syn_clause_groupings.end());
  }
  return clause_groupings;
}

void QueryOptimizer::FindConnectedGroups(
    ClauseGroup *clause_group,
    std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause,
    std::unordered_map<std::string, bool> *has_visited_syn,
    std::unordered_map<int, bool> *has_visited_clause, std::string curr_syn) {
  has_visited_syn->at(curr_syn) = true;
  std::vector<std::string> group_syn_used = clause_group->get_syn_used();
  if (std::find(group_syn_used.begin(), group_syn_used.end(), curr_syn) == group_syn_used.end()) {
    // Add current synonym to synonyms used vector in clause_group if it doesn't exist.
    clause_group->AddSynUsed(curr_syn);
  }

  // Iterate through clauses that contain that synonym and add those clauses to the clause group.
  for (ClauseVertex clause_vertex : syn_to_clause[curr_syn]) {
    int clause_vertex_id = clause_vertex.get_id();
    if (has_visited_clause->at(clause_vertex_id)) {
      // If we have added this clause then move on to next clause vertex.
      continue;
    }
    clause_group->AddClause(clause_vertex);
    has_visited_clause->at(clause_vertex_id) = true;
    std::vector<std::string> syn_used = clause_vertex.get_synonyms_used();
    for (std::string syn : syn_used) {
      if (!has_visited_syn->at(syn)) {
        // Finding other synonyms that haven't been visited and has been used in the same clause.
        FindConnectedGroups(clause_group, syn_to_clause, has_visited_syn, has_visited_clause, syn);
      }
    }
  }
}

int QueryOptimizer::AssignPriority(std::vector<std::string> synonyms_used,
                                   std::shared_ptr<Clause> clause) {
  if (clause->get_type() == ClauseType::WithClause) {
    return 0;
  } else if (clause->get_type() == ClauseType::SuchThatClause) {
    std::shared_ptr<SuchThatClause> such_that_clause =
        std::dynamic_pointer_cast<SuchThatClause>(clause);
    // Assigning priority based on number of synonyms used and such that clause type.
    if (synonyms_used.size() == 1 && (such_that_clause->get_type() == RelRef::Follows ||
                                      such_that_clause->get_type() == RelRef::Parent ||
                                      such_that_clause->get_type() == RelRef::UsesP ||
                                      such_that_clause->get_type() == RelRef::UsesS ||
                                      such_that_clause->get_type() == RelRef::ModifiesP ||
                                      such_that_clause->get_type() == RelRef::ModifiesS ||
                                      such_that_clause->get_type() == RelRef::Next ||
                                      such_that_clause->get_type() == RelRef::Calls)) {
      return 1;
    } else if (synonyms_used.size() == 1 && (such_that_clause->get_type() == RelRef::FollowsT ||
                                             such_that_clause->get_type() == RelRef::ParentT ||
                                             such_that_clause->get_type() == RelRef::NextT ||
                                             such_that_clause->get_type() == RelRef::CallsT ||
                                             such_that_clause->get_type() == RelRef::Affects ||
                                             such_that_clause->get_type() == RelRef::AffectsT)) {
      return 2;
    } else if (synonyms_used.size() == 2 && (such_that_clause->get_type() == RelRef::Follows ||
                                             such_that_clause->get_type() == RelRef::Parent ||
                                             such_that_clause->get_type() == RelRef::UsesP ||
                                             such_that_clause->get_type() == RelRef::UsesS ||
                                             such_that_clause->get_type() == RelRef::ModifiesP ||
                                             such_that_clause->get_type() == RelRef::ModifiesS ||
                                             such_that_clause->get_type() == RelRef::Next ||
                                             such_that_clause->get_type() == RelRef::Calls)) {
      return 3;
    } else if (synonyms_used.size() == 2 && (such_that_clause->get_type() == RelRef::FollowsT ||
                                             such_that_clause->get_type() == RelRef::ParentT ||
                                             such_that_clause->get_type() == RelRef::NextT ||
                                             such_that_clause->get_type() == RelRef::CallsT ||
                                             such_that_clause->get_type() == RelRef::Affects ||
                                             such_that_clause->get_type() == RelRef::AffectsT)) {
      return 4;
    }
  } else if (clause->get_type() == ClauseType::PatternClause) {
    if (synonyms_used.size() == 1) {
      return 1;
    } else if (synonyms_used.size() == 2) {
      return 3;
    }
  }
}

std::shared_ptr<ClauseGroup> QueryOptimizer::MakeNoReturnSynGroup(
    std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause,
    std::unordered_map<int, bool> *has_visited_clause) {
  // Constructs vector of clauses with no return synonym used and creates a group.
  std::vector<ClauseVertex> no_return_syn_used;
  for (auto pair : syn_to_clause) {
    for (ClauseVertex clause_vertex : pair.second) {
      // Adding clause vertex to no return syn if it hasn't been added before and
      if (!has_visited_clause->at(clause_vertex.get_id())) {
        no_return_syn_used.push_back(clause_vertex);
        has_visited_clause->at(clause_vertex.get_id()) = true;
      }
    }
  }
  ClauseGroup clause_group = ClauseGroup();
  clause_group.set_clauses(no_return_syn_used);
  clause_group.SortWithinGroup();
  std::shared_ptr<ClauseGroup> no_syn_group_ptr = std::make_shared<ClauseGroup>(clause_group);
  return no_syn_group_ptr;
}

std::vector<ClauseVertex> QueryOptimizer::MakeSuchThatVertices(std::vector<SuchThatClause *> *such_that_clauses) {
  // Iterating through such that clauses to build a vector of clause vertices.
  std::vector<ClauseVertex> clause_vertices;
  for (SuchThatClause *such_that_clause : *such_that_clauses) {
    std::shared_ptr<Clause> clause_ptr(such_that_clause);
    clause_ptr->set_type(ClauseType::SuchThatClause);

    std::vector<std::string> synonyms_used;
    bool has_return_syn = false;
    SuchThatRef *left_ent = such_that_clause->get_left_ref();
    SuchThatRef *right_ent = such_that_clause->get_right_ref();

    std::string left_syn = "";
    // If left arg is a synonym, add to synonyms used
    if (left_ent->get_type() == SuchThatRefType::Statement &&
        left_ent->get_stmt_ref().get_type() == StmtRefType::Synonym) {
      left_syn = left_ent->get_stmt_ref().get_synonym();
    } else if (left_ent->get_type() == SuchThatRefType::Entity &&
        left_ent->get_ent_ref().get_type() == EntRefType::Synonym) {
      left_syn = left_ent->get_ent_ref().get_synonym();
    } else if (left_ent->get_type() == SuchThatRefType::Line &&
        left_ent->get_line_ref().get_type() == LineRefType::Synonym) {
      left_syn = left_ent->get_line_ref().get_synonym();
    }
    if (left_syn != "") {
      synonyms_used.push_back(left_syn);
      // If synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(left_syn)) {
        has_return_syn = true;
      }
    }

    std::string right_syn = "";
    // If right arg is a synonym, add to synonyms used.
    if (right_ent->get_type() == SuchThatRefType::Statement &&
        right_ent->get_stmt_ref().get_type() == StmtRefType::Synonym) {
      right_syn = right_ent->get_stmt_ref().get_synonym();
    } else if (right_ent->get_type() == SuchThatRefType::Entity &&
        right_ent->get_ent_ref().get_type() == EntRefType::Synonym) {
      right_syn = right_ent->get_ent_ref().get_synonym();
    } else if (right_ent->get_type() == SuchThatRefType::Line &&
        right_ent->get_line_ref().get_type() == LineRefType::Synonym) {
      right_syn = right_ent->get_line_ref().get_synonym();
    }
    if (right_syn != "") {
      synonyms_used.push_back(right_syn);
      // If synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(right_syn)) {
        has_return_syn = true;
      }
    }

    // Creating clause vertex and assigning priority to it.
    ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
    clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
    clause_vertex.set_id(count);
    count++;
    clause_vertices.push_back(clause_vertex);
  }
  return clause_vertices;
}

std::vector<ClauseVertex> QueryOptimizer::MakePatternVertices(std::vector<PatternClause *> *pattern_clauses) {
  std::vector<ClauseVertex> clause_vertices;
  for (PatternClause *pattern_clause : *pattern_clauses) {
    std::shared_ptr<Clause> clause_ptr(pattern_clause);
    clause_ptr->set_type(ClauseType::PatternClause);

    std::vector<std::string> synonyms_used;
    bool has_return_syn = false;
    EntityDeclaration *left_ent = pattern_clause->get_synonym();
    EntRef *right_ent = pattern_clause->get_variable();

    // If left arg is a synonym, then add to synonyms_used.
    std::string left_syn = left_ent->get_synonym();
    synonyms_used.push_back(left_syn);
    // If left synonym is part of return synonyms, set has_return_syn to true.
    if (ReturnEntitiesContainSynonym(left_syn)) {
      has_return_syn = true;
    }

    // If left arg is a synonym, then add to synonyms_used.
    if (right_ent->get_type() == EntRefType::Synonym) {
      std::string right_syn = right_ent->get_synonym();
      synonyms_used.push_back(right_syn);
      // If right synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(right_syn)) {
        has_return_syn = true;
      }
    }

    // Creating clause vertex and assigning priority to it.
    ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
    clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
    clause_vertex.set_id(count);
    count++;
    clause_vertices.push_back(clause_vertex);
  }
  return clause_vertices;
}

std::vector<ClauseVertex> QueryOptimizer::MakeWithVertices(std::vector<WithClause *> *with_clauses) {
  std::vector<ClauseVertex> clause_vertices;
  for (WithClause *with_clause : *with_clauses) {
    std::shared_ptr<Clause> clause_ptr(with_clause);
    clause_ptr->set_type(ClauseType::WithClause);

    std::vector<std::string> synonyms_used;
    bool has_return_syn = false;
    // If left arg is a synonym, add to synonyms_used.
    if (with_clause->get_left_type() != EntityType::None) {
      std::string left_syn = with_clause->get_left_ref();
      synonyms_used.push_back(left_syn);
      // If left synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(left_syn)) {
        has_return_syn = true;
      }
    }
    // If right arg is a synonym, add to synonyms_used.
    if (with_clause->get_right_type() != EntityType::None) {
      std::string right_syn = with_clause->get_right_ref();
      synonyms_used.push_back(right_syn);
      // If left synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(right_syn)) {
        has_return_syn = true;
      }
    }

    // Creating clause vertex and assigning priority to it.
    ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
    clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
    clause_vertex.set_id(count);
    count++;
    clause_vertices.push_back(clause_vertex);
    }
  return clause_vertices;
}

std::vector<ClauseVertex> QueryOptimizer::CombineAllVertices(std::vector<ClauseVertex> such_that_vertices,
                                                             std::vector<ClauseVertex> pattern_vertices,
                                                             std::vector<ClauseVertex> with_vertices) {
  std::vector<ClauseVertex> all_vertices;
  // Appending vertices to vector.
  all_vertices.insert(all_vertices.begin(), such_that_vertices.begin(), such_that_vertices.end());
  all_vertices.insert(all_vertices.end(), pattern_vertices.begin(), pattern_vertices.end());
  all_vertices.insert(all_vertices.end(), with_vertices.begin(), with_vertices.end());
  return all_vertices;
}

bool QueryOptimizer::ReturnEntitiesContainSynonym(std::string s) {
  for (ResultClause *result : *return_entities_) {
    if (result->get_synonym() == s) {
      return true;
    }
  }
  return false;
}

#include <algorithm>

#include "query_optimizer.h"
#include <iterator>

QueryOptimizer::QueryOptimizer(std::vector<SuchThatClause *> *relationships,
                               std::vector<PatternClause *> *patterns,
                               std::vector<WithClause *> *withs,
                               std::vector<ResultClause *> *return_entities) {
  this->relationships_ = relationships;
  this->patterns_ = patterns;
  this->withs_ = withs;
  this->return_entities_ = return_entities;
  this->count_ = 0;
}

// Optimizer is "off" and will create 3 groups with only the last group filled.
std::vector<std::shared_ptr<ClauseGroup>> QueryOptimizer::CreateDefaultGroupings() {
  std::vector<std::shared_ptr<ClauseGroup>> clause_groupings;

  // Creating empty groups for first 2 groups
  clause_groupings.push_back(std::make_shared<ClauseGroup>(ClauseGroup()));
  clause_groupings.push_back(std::make_shared<ClauseGroup>(ClauseGroup()));

  std::vector<ClauseVertex> all_vertices = CombineAllVertices(
      MakeSuchThatVertices(relationships_),
      MakePatternVertices(patterns_),
      MakeWithVertices(withs_));
  auto all_clauses = ClauseGroup();
  all_clauses.set_clauses(all_vertices);
  auto all_clauses_ptr = std::make_shared<ClauseGroup>(all_clauses);
  clause_groupings.push_back(all_clauses_ptr);

  return clause_groupings;
}

std::vector<std::shared_ptr<ClauseGroup>> QueryOptimizer::CreateOptimizedGroupings() {
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
               std::back_inserter(no_syn_used),
               [](auto vertex) { return vertex.get_synonyms_used().empty(); });
  // Add to has_syn_used if clause vertex uses synonyms.
  std::copy_if(all_vertices.begin(),
               all_vertices.end(),
               std::back_inserter(has_syn_used),
               [](auto vertex) { return !vertex.get_synonyms_used().empty(); });
  // Add to has_return_syn_used if clause vertex uses return synonyms.
  std::copy_if(all_vertices.begin(),
               all_vertices.end(),
               std::back_inserter(has_return_syn_used),
               [](auto vertex) { return vertex.get_has_return_syn(); });

  // Group with no synonyms will be evaluated first
  auto no_syn_group = ClauseGroup();
  no_syn_group.set_clauses(no_syn_used);
//  no_syn_group.SortWithinGroup();
  std::shared_ptr<ClauseGroup> no_syn_group_ptr = std::make_shared<ClauseGroup>(no_syn_group);
  clause_groupings.push_back(no_syn_group_ptr);

  // Group rest of vertices based on connected synonyms
  // Create unordered_map mapping synonym to the clauseVertex it appears in.
  // Also create a has_visited_syn map to map each synonym to a boolean.
  std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause;
  std::unordered_map<std::string, std::vector<ClauseVertex>> return_syn_to_clause;
  auto *has_visited_syn = new std::unordered_map<std::string, bool>();
  auto *has_visited_clause = new std::unordered_map<int, bool>();

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
        // If map does not contain syn and clause vertex has return syn
        syn_to_clause.insert({syn, {clause_vertex}});
        return_syn_to_clause.insert({syn, {clause_vertex}});
        has_visited_syn->insert({syn, false});
      } else {
        // If map does not contain syn and clause vertex has no return syn
        syn_to_clause.insert({syn, {clause_vertex}});
        has_visited_syn->insert({syn, false});
      }
    }
  }

  // Create vector of ClauseGroups for return syn and no return syns.
  std::vector<std::shared_ptr<ClauseGroup>> return_syn_clause_groupings =
      MakeClauseGroupings(&return_syn_to_clause, &syn_to_clause, has_visited_syn, has_visited_clause);
  std::vector<std::shared_ptr<ClauseGroup>> no_return_syn_clause_groupings =
      MakeClauseGroupings(&syn_to_clause, &syn_to_clause, has_visited_syn, has_visited_clause);

  // Add no return syn groups to clause groupings first, then has return syn groups
  clause_groupings.insert(clause_groupings.end(), no_return_syn_clause_groupings.begin(),
                          no_return_syn_clause_groupings.end());
  clause_groupings.insert(clause_groupings.end(), return_syn_clause_groupings.begin(),
                          return_syn_clause_groupings.end());

  return clause_groupings;
}

std::vector<std::shared_ptr<ClauseGroup>> QueryOptimizer::MakeClauseGroupings(
    std::unordered_map<std::string, std::vector<ClauseVertex>> *vertices_to_iterate,
    std::unordered_map<std::string, std::vector<ClauseVertex>> *syn_to_clause,
    std::unordered_map<std::string, bool> *has_visited_syn,
    std::unordered_map<int, bool> *has_visited_clause) {
  std::vector<std::shared_ptr<ClauseGroup>> groupings;
  // iterate through return synonyms and run dfs to group clauses based on connected synonyms.
  for (auto pair : *vertices_to_iterate) {
    if (!has_visited_syn->at(pair.first)) {
      // run dfs if synonym has not been visited
      ClauseGroup *group = new ClauseGroup();
      FindConnectedGroups(group,
                          *syn_to_clause,
                          has_visited_syn,
                          has_visited_clause,
                          pair.first);

      group->SortWithinGroup();
      std::shared_ptr<ClauseGroup> connected_syn_group_ptr =
          std::make_shared<ClauseGroup>(*group);
      groupings.push_back(connected_syn_group_ptr);
    }
  }
  return groupings;
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

  // Check if current syn is a return syn. If it is, then set has_return_syn for group to true
  if (ReturnEntitiesContainSynonym(curr_syn)) {
    clause_group->set_has_return_syn(true);
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
        FindConnectedGroups(clause_group,
                            syn_to_clause,
                            has_visited_syn,
                            has_visited_clause,
                            syn);
      }
    }
  }
}

//    Group 1: with (1 syn)
//    Group 2: with (2 syn)
//    Group 3: one syn follows, parent, next
//    Group 4: one syn usesS, usesP, modifiesS, modifiesP, calls, pattern
//    Group 5: one syn follows*, parent*, next*, calls*, affects, affects*
//    Group 6: one syn affectsbip, nextbip
//    Group 7: one syn affectsbip*, nextbip*
//    Group 8: two syn follows, parent, next
//    Group 9: two syn usesS, usesP, modifiesS, modifiesP, calls, pattern
//    Group 10: two syn follows*, parent*, next*, calls*, affects, affects*
//    Group 11: two syn affectsbip, nextbip
//    Group 12: two syn affectsbip*, nextbip*
int QueryOptimizer::AssignPriority(std::vector<std::string> synonyms_used,
                                   std::shared_ptr<Clause> clause) {
  if (clause->get_type() == ClauseType::WithClause) {
    if (synonyms_used.size() == 1) {
      return 0;
    } else {
      return 1;
    }
  } else if (clause->get_type() == ClauseType::SuchThatClause) {
    std::shared_ptr<SuchThatClause> such_that_clause =
        std::dynamic_pointer_cast<SuchThatClause>(clause);
    // Assigning priority based on number of synonyms used and such that clause type.
    if (such_that_clause->get_type() == RelRef::Follows ||
        such_that_clause->get_type() == RelRef::Parent ||
        such_that_clause->get_type() == RelRef::Next) {
      if (synonyms_used.size() == 1) {
        return 3;
      } else if (synonyms_used.size() == 2) {
        return 8;
      }
    } else if (such_that_clause->get_type() == RelRef::UsesP ||
        such_that_clause->get_type() == RelRef::UsesS ||
        such_that_clause->get_type() == RelRef::ModifiesP ||
        such_that_clause->get_type() == RelRef::ModifiesS ||
        such_that_clause->get_type() == RelRef::Calls) {
      if (synonyms_used.size() == 1) {
        return 4;
      } else if (synonyms_used.size() == 2) {
        return 9;
      }
    } else if (such_that_clause->get_type() == RelRef::FollowsT ||
        such_that_clause->get_type() == RelRef::ParentT ||
        such_that_clause->get_type() == RelRef::NextT ||
        such_that_clause->get_type() == RelRef::CallsT ||
        such_that_clause->get_type() == RelRef::Affects ||
        such_that_clause->get_type() == RelRef::AffectsT) {
      if (synonyms_used.size() == 1) {
        return 5;
      } else if (synonyms_used.size() == 2) {
        return 10;
      }
    } else if (such_that_clause->get_type() == RelRef::AffectsBip ||
        such_that_clause->get_type() == RelRef::NextBip) {
      if (synonyms_used.size() == 1) {
        return 6;
      } else if (synonyms_used.size() == 2) {
        return 11;
      }
    } else if (such_that_clause->get_type() == RelRef::AffectsTBip ||
        such_that_clause->get_type() == RelRef::NextTBip) {
      if (synonyms_used.size() == 1) {
        return 7;
      } else if (synonyms_used.size() == 2) {
        return 12;
      }
    }
  } else if (clause->get_type() == ClauseType::PatternClause) {
    if (synonyms_used.size() == 1) {
      return 4;
    } else if (synonyms_used.size() == 2) {
      return 9;
    }
  }
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

    std::string left_syn;
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
    if (!left_syn.empty()) {
      synonyms_used.push_back(left_syn);
      // If synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(left_syn)) {
        has_return_syn = true;
      }
    }

    std::string right_syn;
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
    if (!right_syn.empty()) {
      synonyms_used.push_back(right_syn);
      // If synonym is part of return synonyms, set has_return_syn to true.
      if (ReturnEntitiesContainSynonym(right_syn)) {
        has_return_syn = true;
      }
    }

    // Creating clause vertex and assigning priority to it.
    ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
    clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
    clause_vertex.set_id(count_);
    count_++;
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
    clause_vertex.set_id(count_);
    count_++;
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
    clause_vertex.set_id(count_);
    count_++;
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

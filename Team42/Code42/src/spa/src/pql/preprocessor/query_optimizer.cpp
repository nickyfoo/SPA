#include "query_optimizer.h"

QueryOptimizer::QueryOptimizer(std::vector<SuchThatClause *> *relationships,
                               std::vector<PatternClause *> *patterns,
                               std::vector<WithClause *> *withs,
                               std::vector<std::string> *return_entities) {
  printf("here5\n");
  this->relationships_ = relationships;
  this->patterns_ = patterns;
  printf("here6\n");
  this->withs_ = withs;
  this->return_entities_ = return_entities;
}

std::vector<std::shared_ptr<ClauseGroup>> QueryOptimizer::CreateGroupings() {
  std::vector<std::shared_ptr<ClauseGroup>> clause_groupings;
  std::vector<ClauseVertex> no_syn_used;
  std::vector<ClauseVertex> no_return_syn_used;
  std::vector<ClauseVertex> has_return_syn_used;
  int count = 0;

  // Iterating through relationship, pattern and with vectors
  // and creating ClauseVertexes.
  for (SuchThatClause *relationship : *relationships_) {
    ClauseVertex such_that_vertex = MakeSuchThatVertex(relationship);
    such_that_vertex.set_id(count);
    count++;
    // If there are no synonyms used in this clause,
    // add to no_syn_used vector
    if (such_that_vertex.get_synonyms_used().empty()) {
      no_syn_used.push_back(such_that_vertex);
    } else if (!such_that_vertex.get_has_return_syn()) {
      // If there are no return synonyms in the clause,
      // add to no_return_syn_used vector
      no_return_syn_used.push_back(such_that_vertex);
    } else {
      has_return_syn_used.push_back(such_that_vertex);
    }
  }
  for (PatternClause *pattern_clause : *patterns_) {
    ClauseVertex pattern_vertex = MakePatternVertex(pattern_clause);
    pattern_vertex.set_id(count);
    count++;
    // If there are no synonyms used in this clause,
    // add to no_syn_used vector
    if (pattern_vertex.get_synonyms_used().empty()) {
      no_syn_used.push_back(pattern_vertex);
    } else if (!pattern_vertex.get_has_return_syn()) {
      // If there are no return synonyms in the clause,
      // add to no_return_syn_used vector
      no_return_syn_used.push_back(pattern_vertex);
    } else {
      has_return_syn_used.push_back(pattern_vertex);
    }
  }
  for (WithClause *with_clause : *withs_) {
    ClauseVertex with_vertex = MakeWithVertex(with_clause);
    with_vertex.set_id(count);
    count++;
    // If there are no synonyms used in this clause,
    // add to no_syn_used vector
    if (with_vertex.get_synonyms_used().empty()) {
      no_syn_used.push_back(with_vertex);
    } else if (!with_vertex.get_has_return_syn()) {
      // If there are no return synonyms in the clause,
      // add to no_return_syn_used vector
      no_return_syn_used.push_back(with_vertex);
    } else {
      has_return_syn_used.push_back(with_vertex);
    }
  }

  // Group with no synonyms will be evaluated first
  auto no_syn_group = ClauseGroup();
  no_syn_group.set_clauses(no_syn_used);
  no_syn_group.SortWithinGroup();
  std::shared_ptr<ClauseGroup> no_syn_group_ptr = std::make_shared<ClauseGroup>(no_syn_group);
  clause_groupings.push_back(no_syn_group_ptr);

  // Group with no return synonym will be evaluated next
  auto no_return_syn_group = ClauseGroup();
  no_return_syn_group.set_clauses(no_return_syn_used);
  no_return_syn_group.SortWithinGroup();
  std::shared_ptr<ClauseGroup> no_return_syn_group_ptr =
      std::make_shared<ClauseGroup>(no_return_syn_group);
  clause_groupings.push_back(no_return_syn_group_ptr);

  // If there are no clauses with return synonym, add an empty group to the vector.
  if (has_return_syn_used.empty()) {
    auto has_return_syn_group = ClauseGroup();
    std::shared_ptr<ClauseGroup> has_return_syn_group_ptr =
        std::make_shared<ClauseGroup>(has_return_syn_group);
    clause_groupings.push_back(has_return_syn_group_ptr);
  }

  // Group rest of vertices based on connected synonyms
  // Create unordered_map mapping synonym to the clauseVertex it appears in.
  // Also create a has_visited_syn map to map each synonym to a boolean.
  std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause;
  auto *has_visited_syn = new std::unordered_map<std::string, bool>();
  auto *has_visited_clause = new std::unordered_map<int, bool>();
  for (ClauseVertex clause_vertex : has_return_syn_used) {
    has_visited_clause->insert({clause_vertex.get_id(), false});
    for (std::string syn : clause_vertex.get_synonyms_used()) {
      if (syn_to_clause.find(syn) != syn_to_clause.end()) {
        syn_to_clause[syn].push_back(clause_vertex);
      } else {
        syn_to_clause.insert({syn, {clause_vertex}});
        has_visited_syn->insert({syn, false});
      }
    }
  }

  // iterate through synonyms and run dfs to group clauses based on connected synonyms.
  for (auto pair : syn_to_clause) {
    if (!has_visited_syn->at(pair.first)) {
      // run dfs if synonym has not been visited
      ClauseGroup *has_return_syn_group = new ClauseGroup();
      FindConnectedGroups(has_return_syn_group,
                          syn_to_clause,
                          has_visited_syn,
                          has_visited_clause,
                          pair.first);
      has_return_syn_group->SortWithinGroup();
      std::shared_ptr<ClauseGroup> connected_syn_group_ptr =
          std::make_shared<ClauseGroup>(*has_return_syn_group);
      clause_groupings.push_back(connected_syn_group_ptr);
    }
  }
  return clause_groupings;
}

void QueryOptimizer::FindConnectedGroups(
    ClauseGroup *clause_group,
    std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause,
    std::unordered_map<std::string, bool> *has_visited_syn,
    std::unordered_map<int, bool> *has_visited_clause,
    std::string curr_syn) {
  has_visited_syn->at(curr_syn) = true;
  std::vector<std::string> group_syn_used = clause_group->get_syn_used();
  if (std::find(group_syn_used.begin(), group_syn_used.end(), curr_syn) ==
      group_syn_used.end()) {
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
        FindConnectedGroups(clause_group,
                            syn_to_clause,
                            has_visited_syn,
                            has_visited_clause,
                            syn);
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
    if (synonyms_used.size() == 1 &&
        (such_that_clause->get_type() == RelRef::Follows ||
            such_that_clause->get_type() == RelRef::Parent ||
            such_that_clause->get_type() == RelRef::UsesP ||
            such_that_clause->get_type() == RelRef::UsesS ||
            such_that_clause->get_type() == RelRef::ModifiesP ||
            such_that_clause->get_type() == RelRef::ModifiesS ||
            such_that_clause->get_type() == RelRef::Next ||
            such_that_clause->get_type() == RelRef::Calls)) {
      return 1;
    } else if (synonyms_used.size() == 1 &&
        (such_that_clause->get_type() == RelRef::FollowsT ||
            such_that_clause->get_type() == RelRef::ParentT ||
            such_that_clause->get_type() == RelRef::NextT ||
            such_that_clause->get_type() == RelRef::CallsT ||
            such_that_clause->get_type() == RelRef::Affects ||
            such_that_clause->get_type() == RelRef::AffectsT)) {
      return 2;
    } else if (synonyms_used.size() == 2 &&
        (such_that_clause->get_type() == RelRef::Follows ||
            such_that_clause->get_type() == RelRef::Parent ||
            such_that_clause->get_type() == RelRef::UsesP ||
            such_that_clause->get_type() == RelRef::UsesS ||
            such_that_clause->get_type() == RelRef::ModifiesP ||
            such_that_clause->get_type() == RelRef::ModifiesS ||
            such_that_clause->get_type() == RelRef::Next ||
            such_that_clause->get_type() == RelRef::Calls)) {
      return 3;
    } else if (synonyms_used.size() == 2 &&
        (such_that_clause->get_type() == RelRef::FollowsT ||
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

ClauseVertex QueryOptimizer::MakeSuchThatVertex(SuchThatClause *such_that_clause) {
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
    if (std::find(return_entities_->begin(), return_entities_->end(), left_syn) !=
    return_entities_->end()) {
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
    if (std::find(return_entities_->begin(), return_entities_->end(), right_syn) !=
    return_entities_->end()) {
      has_return_syn = true;
    }
  }

  // Creating clause vertex and assigning priority to it.
  ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
  clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
  return clause_vertex;
}

ClauseVertex QueryOptimizer::MakePatternVertex(PatternClause *pattern_clause) {
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
  if (std::find(return_entities_->begin(), return_entities_->end(), left_syn) !=
  return_entities_->end()) {
    has_return_syn = true;
  }

  // If left arg is a synonym, then add to synonyms_used.
  if (right_ent->get_type() == EntRefType::Synonym) {
    std::string right_syn = right_ent->get_synonym();
    synonyms_used.push_back(right_syn);
    // If right synonym is part of return synonyms, set has_return_syn to true.
    if (std::find(return_entities_->begin(), return_entities_->end(), right_syn) !=
    return_entities_->end()) {
      has_return_syn = true;
    }
  }

  // Creating clause vertex and assigning priority to it.
  ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
  clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
  return clause_vertex;
}

ClauseVertex QueryOptimizer::MakeWithVertex(WithClause *with_clause) {
  std::shared_ptr<Clause> clause_ptr(with_clause);
  clause_ptr->set_type(ClauseType::WithClause);

  std::vector<std::string> synonyms_used;
  bool has_return_syn = false;

  // If left arg is a synonym, add to synonyms_used.
  if (with_clause->get_left_type() != EntityType::None) {
    std::string left_syn = with_clause->get_left_ref();
    synonyms_used.push_back(left_syn);
    // If left synonym is part of return synonyms, set has_return_syn to true.
    if (std::find(return_entities_->begin(), return_entities_->end(), left_syn) !=
    return_entities_->end()) {
      has_return_syn = true;
    }
  }

  // If right arg is a synonym, add to synonyms_used.
  if (with_clause->get_right_type() != EntityType::None) {
    std::string right_syn = with_clause->get_right_ref();
    synonyms_used.push_back(right_syn);
    // If left synonym is part of return synonyms, set has_return_syn to true.
    if (std::find(return_entities_->begin(), return_entities_->end(), right_syn) !=
    return_entities_->end()) {
      has_return_syn = true;
    }
  }

  // Creating clause vertex and assigning priority to it.
  ClauseVertex clause_vertex = ClauseVertex(synonyms_used, has_return_syn, clause_ptr);
  clause_vertex.set_priority(AssignPriority(synonyms_used, clause_ptr));
  return clause_vertex;
}

#include "pattern_query_manager.h"
#include "entity_declaration.h"
#include "stmt_table.h"
#include "statement.h"
#include <set>

PatternQueryManager::PatternQueryManager(
    std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result,
    std::vector<PatternClause *> *patterns,
    std::vector<std::string> *entities_to_return,
    PKB *pkb,
    bool has_two_repeated_synonyms) {
  this->synonym_to_entity_result_ = synonym_to_entity_result;
  this->patterns_ = patterns;
  this->entities_to_return_ = entities_to_return;
  this->pkb_ = pkb;
  this->vec_results_ = new std::vector<std::pair<int, std::string>>();
  this->has_two_repeated_synonyms_ = has_two_repeated_synonyms;
}

PatternQueryManager::~PatternQueryManager() = default;

void PatternQueryManager::EvaluatePatterns() {
  // Iterating through patterns_ and evaluating one by one.
  // For basic requirements, there will be only 1 pattern.
  for (PatternClause *pattern : *patterns_) {
    ManagePatterns(pattern);
  }
}

void PatternQueryManager::ManagePatterns(PatternClause *pattern) {
  EntityDeclaration *synonym = pattern->get_synonym();
  EntRef *left_ent = pattern->get_variable();
  ExpressionSpec *right_ent = pattern->get_exp_spec();
  std::string pattern_to_check = right_ent->get_expression();
  bool is_partial_pattern = right_ent->IsPartialPattern();

  // list of assignment object
  std::vector<Entity *> *entity_vec = &synonym_to_entity_result_->at(synonym->get_synonym());
  std::set<Entity *> var_set;
  std::string left_synonym;

  for (int i = 0; i < entity_vec->size(); i++) {
    auto *assign = dynamic_cast<Statement *>(entity_vec->at(i));  // for each assign object
    if ((assign->get_modifies()->size() == 0
        || !pkb_->TestAssignmentPattern(assign, pattern_to_check, is_partial_pattern))
        && !right_ent->IsWildCard()) {
      entity_vec->erase(entity_vec->begin() + i);
      i--;
    } else {
      if (left_ent->get_type() == EntRefType::Synonym) {  // pattern a(v, "pattern")
        left_synonym = left_ent->get_synonym();
        // getting list of variable objects
        std::vector<Entity *> *variable_vec =
            &synonym_to_entity_result_->at(left_ent->get_synonym());
        if (has_two_repeated_synonyms_) {
          // for each variable object
          std::set<std::string> *modified_set = assign->get_modifies();
          for (std::string s :*modified_set) {
            vec_results_->push_back(make_pair(assign->get_stmt_no(), s));
          }
        } else {
          // for each variable object
          bool found_variable = false;
          for (int j = 0; j < variable_vec->size(); j++) {
            auto *variable = dynamic_cast<Variable *>(variable_vec->at(j));
            if (assign->get_modifies()->count(variable->get_name())) {
              found_variable = true;
              var_set.insert(variable_vec->at(j));
            }
          }
          if (!found_variable) {
            entity_vec->erase(entity_vec->begin() + i);
            i--;
          }
        }
      } else if (left_ent->get_type() == EntRefType::Argument) {
        if (!assign->get_modifies()->count(left_ent->get_argument())) {
          entity_vec->erase(entity_vec->begin() + i);
          i--;
        }
      }
    }
  }
  if (!var_set.empty()) {
    std::vector<Entity *> *variable_vec = &synonym_to_entity_result_->at(left_synonym);
    // getting list of variable objects
    for (int k = 0; k < variable_vec->size(); k++) {
      if (!var_set.count(variable_vec->at(k))) {
        variable_vec->erase(variable_vec->begin() + k);
        k--;
      }
    }
  } else if (var_set.empty() && left_ent->get_type() == EntRefType::Synonym
  && !has_two_repeated_synonyms_) {
    synonym_to_entity_result_->clear();
  }
}

std::vector<std::pair<int, std::string>> *PatternQueryManager::get_vec_results() {
  return vec_results_;
}

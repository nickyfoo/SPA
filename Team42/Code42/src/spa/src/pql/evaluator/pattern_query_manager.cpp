#include "pattern_query_manager.h"
#include "entity_declaration.h"
#include "statement.h"
#include <set>

PatternQueryManager::PatternQueryManager(PKB *pkb) {
  this->pkb_ = pkb;
}

PatternQueryManager::~PatternQueryManager() = default;


ResultTable *PatternQueryManager::EvaluatePattern(std::shared_ptr<PatternClause> pattern,
                                                  const std::unordered_map<std::string,
                                                  std::vector<Entity *>> &synonym_to_entities_vec) {
  if (pattern->get_type() == EntityType::Assign) {
    return EvaluateAssignPattern(pattern, synonym_to_entities_vec);
  } else if (pattern->get_type() == EntityType::If || pattern->get_type() == EntityType::While) {
    return EvaluateIfAndWhilePattern(pattern, synonym_to_entities_vec);
  } else {
    throw std::runtime_error("Unknown pattern type!");
  }
}

ResultTable *PatternQueryManager::EvaluateAssignPattern(
    std::shared_ptr<PatternClause> pattern, std::unordered_map<std::string,
    std::vector<Entity *>> synonym_to_entities_vec) {
  auto *ret = new ResultTable();
  EntityDeclaration *synonym = pattern->get_synonym();
  EntRef *left_ent = pattern->get_variable();
  ExpressionSpec *right_ent = pattern->get_exp_spec();
  std::string pattern_to_check = right_ent->get_expression();
  bool is_partial_pattern = right_ent->IsPartialPattern();

  // list of assignment object
  std::vector<Entity *> entity_vec = synonym_to_entities_vec.at(synonym->get_synonym());
  std::set<Entity *> var_set;
  std::string left_synonym;
  std::vector<std::string> stmt_vec;
  std::vector<std::string> var_vec;

  for (int i = 0; i < entity_vec.size(); i++) {
    auto *stmt = dynamic_cast<Statement *>(entity_vec.at(i));  // for each stmt object
    if ((stmt->get_modifies()->empty()
        || !pkb_->TestAssignmentPattern(stmt, pattern_to_check, is_partial_pattern))
        && !right_ent->IsWildCard()) {
      continue;
    } else {
      if (left_ent->get_type() == EntRefType::Synonym) {  // pattern a(v, "pattern")
        left_synonym = left_ent->get_synonym();

        // getting list of variable objects
        std::vector<Entity *> variable_vec =
            synonym_to_entities_vec.at(left_ent->get_synonym());

        std::set<std::string> *modified_set = stmt->get_modifies();
        for (const std::string &s : *modified_set) {
          stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
          var_vec.push_back(s);
        }
      } else if (left_ent->get_type() == EntRefType::Argument) {  // pattern a("x", "pattern")
        if (stmt->get_modifies()->count(left_ent->get_argument())) {
          stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
        }
      } else if (left_ent->get_type() == EntRefType::WildCard) {  // pattern a(_, "pattern")
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
  }
  if (left_ent->get_type() == EntRefType::Synonym) {
    ret->AddDoubleColumns(synonym->get_synonym(), stmt_vec, left_synonym, var_vec);
  } else if (left_ent->get_type() == EntRefType::Argument ||
      left_ent->get_type() == EntRefType::WildCard) {
    ret->AddSingleColumn(synonym->get_synonym(), stmt_vec);
  }

  if (ret->get_table()->empty()) {
    return nullptr;
  }
  return ret;
}

ResultTable *PatternQueryManager::EvaluateIfAndWhilePattern(
    std::shared_ptr<PatternClause> pattern, std::unordered_map<std::string,
    std::vector<Entity *>> synonym_to_entities_vec) {
  auto *ret = new ResultTable();
  EntityDeclaration *synonym = pattern->get_synonym();
  EntRef *variable = pattern->get_variable();
  EntityType type = pattern->get_type();

  // list of assignment object
  std::vector<Entity *> entity_vec = synonym_to_entities_vec.at(synonym->get_synonym());
  std::set<Entity *> var_set;
  std::string left_synonym;
  std::vector<std::string> stmt_vec;
  std::vector<std::string> var_vec;

  for (int i = 0; i < entity_vec.size(); i++) {
    auto *stmt = dynamic_cast<Statement *>(entity_vec.at(i));  // for each stmt object
    if (stmt->get_uses()->empty()) {
      continue;
    }
    if (variable->get_type() == EntRefType::Synonym) {  // pattern if(v, _, _)
      left_synonym = variable->get_synonym();
      // getting list of variable objects
      std::vector<Entity *> variable_vec =
          synonym_to_entities_vec.at(variable->get_synonym());
      for (Entity *entity : variable_vec) {
        auto *variable = dynamic_cast<Variable *>(entity);
        if (pkb_->TestIfWhilePattern(stmt, variable->get_name())) {
          stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
          var_vec.push_back(variable->get_name());
        }
      }
    } else if (variable->get_type() == EntRefType::Argument) {  // pattern if("a", _, _)
      if (pkb_->TestIfWhilePattern(stmt, variable->get_argument())) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    } else if (variable->get_type() == EntRefType::WildCard) {  // pattern if(_, _, _)
      stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
    }
  }
  if (variable->get_type() == EntRefType::Synonym) {
    ret->AddDoubleColumns(synonym->get_synonym(), stmt_vec, left_synonym, var_vec);
  } else if (variable->get_type() == EntRefType::Argument ||
      variable->get_type() == EntRefType::WildCard) {
    ret->AddSingleColumn(synonym->get_synonym(), stmt_vec);
  }

  if (ret->get_table()->empty()) {
    return nullptr;
  }
  return ret;
}

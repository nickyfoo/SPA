#include "pattern_query_manager.h"
#include "entity_declaration.h"
#include "stmt_table.h"
#include "statement.h"
#include <set>

PatternQueryManager::PatternQueryManager(PKB *pkb) {
  this->pkb_ = pkb;
}

PatternQueryManager::~PatternQueryManager() = default;


ResultTable* PatternQueryManager::EvaluatePatterns(PatternClause pattern,
                                                    std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  ResultTable *ret = new ResultTable();
  EntityDeclaration *synonym = pattern.get_synonym();
  EntRef *left_ent = pattern.get_variable();
  ExpressionSpec *right_ent = pattern.get_exp_spec();
  std::string pattern_to_check = right_ent->get_expression();
  bool is_partial_pattern = right_ent->IsPartialPattern();

  // list of assignment object
  std::vector<Entity *> entity_vec = synonym_to_entities_vec.at(synonym->get_synonym());
  std::set<Entity *> var_set;
  std::string left_synonym;
  std::vector<std::string> stmt_vec;
  std::vector<std::string> var_vec;

  for (int i = 0; i < entity_vec.size(); i++) {
    auto *assign = dynamic_cast<Statement *>(entity_vec.at(i));  // for each assign object
    if ((assign->get_modifies()->size() == 0
        || !pkb_->TestAssignmentPattern(assign, pattern_to_check, is_partial_pattern))
        && !right_ent->IsWildCard()) {
      continue;
    } else {
      if (left_ent->get_type() == EntRefType::Synonym) {  // pattern a(v, "pattern")
        left_synonym = left_ent->get_synonym();

        // getting list of variable objects
        std::vector<Entity *> variable_vec =
            synonym_to_entities_vec.at(left_ent->get_synonym());

        std::set<std::string> *modified_set = assign->get_modifies();
        for (std::string s :*modified_set) {
          stmt_vec.push_back(std::to_string(assign->get_stmt_no()));
          var_vec.push_back(s);
        }
      } else if (left_ent->get_type() == EntRefType::Argument) {  // pattern a("x", "pattern")
        if (assign->get_modifies()->count(left_ent->get_argument())) {
          stmt_vec.push_back(std::to_string(assign->get_stmt_no()));
        }
      } else if (left_ent->get_type() == EntRefType::WildCard) {  // pattern a(_, "pattern")
          stmt_vec.push_back(std::to_string(assign->get_stmt_no()));
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
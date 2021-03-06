#include "usess_modifiess_handler.h"

#include <assert.h>

UsesSModifiesSHandler::UsesSModifiesSHandler() = default;
UsesSModifiesSHandler *UsesSModifiesSHandler::instance_ = nullptr;

UsesSModifiesSHandler *UsesSModifiesSHandler::get_instance() {
  if (!instance_) {
    instance_ = new UsesSModifiesSHandler();
  }
  return instance_;
}

void UsesSModifiesSHandler::set_args(PKB *pkb,
                                     std::shared_ptr<SuchThatClause> relationship,
                                     std::unordered_map<std::string,
                                                        std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
}

ResultTable *UsesSModifiesSHandler::EvaluateUsesS() {
  return Evaluate(&Statement::get_uses,
                  &Variable::get_stmts_using);
}

ResultTable *UsesSModifiesSHandler::EvaluateModifiesS() {
  return Evaluate(&Statement::get_modifies,
                  &Variable::get_stmts_modifying);
}

ResultTable *UsesSModifiesSHandler::Evaluate(std::set<std::string> *(Statement::*get_normal)(),
                                             std::set<int> *(Variable::*get_reverse)()) {
  ResultTable *ret = new ResultTable();
  StmtRef left_ent = relationship_->get_left_ref()->get_stmt_ref();
  EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();
  // Going through 6 different cases for UsesS
  if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::Synonym) {  // Uses(s, v)
    std::string left_synonym = left_ent.get_synonym();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> left_stmt_vec;
    std::vector<std::string> right_var_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      for (int j = 0; j < right_entity_vec.size(); j++) {
        auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));
        auto *variable = dynamic_cast<Variable *>(right_entity_vec.at(j));

        assert(stmt != nullptr && variable != nullptr);

        if ((stmt->*get_normal)()->count(variable->get_name())) {
          left_stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
          right_var_vec.push_back(variable->get_name());
        }
      }
    }
    ret->AddDoubleColumns(left_synonym, left_stmt_vec, right_synonym, right_var_vec);
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::WildCard) {  // Uses(s, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> stmt_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));

      assert(stmt != nullptr);

      // Remove each statement that doesnt use anything.
      if (!(stmt->*get_normal)()->empty()) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
    ret->AddSingleColumn(left_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::Argument) {  // Uses(s, "x")
    std::string left_synonym = left_ent.get_synonym();
    std::string right_arg = right_ent.get_argument();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> stmt_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));

      assert(stmt != nullptr);

      if ((stmt->*get_normal)()->count(right_arg)) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
    ret->AddSingleColumn(left_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::Synonym) {  // Uses(4, v)
    int left_arg = left_ent.get_stmt_num();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<std::string> var_vec;

    for (int i = 0; i < right_entity_vec.size(); i++) {
      auto *variable = dynamic_cast<Variable *>(right_entity_vec.at(i));

      assert(variable != nullptr);

      if ((variable->*get_reverse)()->count(left_arg)) {
        var_vec.push_back(variable->get_name());
      }
    }
    ret->AddSingleColumn(right_synonym, var_vec);
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::WildCard) {  // Uses(4, _)
    int left_arg = left_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || (stmt->*get_normal)()->empty()) {
      // If statement with left arg as line number
      // does not use anything then return null ptr
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::Argument) {  // Uses(4, "x")
    int left_arg = left_ent.get_stmt_num();
    std::string right_arg = right_ent.get_argument();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || !(stmt->*get_normal)()->count(right_arg)) {
      // Return null ptr if this relationship_ is false
      return nullptr;
    }
  }
  return ret;
}


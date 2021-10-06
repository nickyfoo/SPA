#include "usess_modifiess_handler.h"

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
                                     std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
}

std::set<std::string> *UsesSModifiesSHandler::StatementForwarder(
    std::set<std::string> *(Statement::*function)(),
    Statement *stmt) {
  return (stmt->*function)();
}

std::set<int> *UsesSModifiesSHandler::VariableForwarder(
    std::set<int> *(Variable::*function)(),
    Variable *var) {
  return (var->*function)();
}

void UsesSModifiesSHandler::set_function_pointers(
    std::set<std::string> *(Statement::*get_normal)(),
    std::set<int> *(Variable::*get_reverse)()) {
  this->get_normal_ = get_normal;
  this->get_reverse_ = get_reverse;
}

ResultTable* UsesSModifiesSHandler::Evaluate() {
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
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));
      for (int j = 0; j < right_entity_vec.size(); j++) {
        auto *variable = dynamic_cast<Variable *>(right_entity_vec.at(j));
        if (stmt != nullptr && variable != nullptr && StatementForwarder(get_normal_, stmt)->count(variable->get_name())) {
          left_stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
          right_var_vec.push_back(variable->get_name());
        }
      }
    }
//    for (int j = 0; j < right_entity_vec.size(); j++) {
//      auto *variable = dynamic_cast<Variable *>(right_entity_vec.at(j));
//      if (variable != nullptr && !VariableForwarder(get_reverse_, variable)->empty()) {
//        right_var_vec.push_back(variable->get_name());
//      }
//    }

    ret->AddDoubleColumns(left_synonym, left_stmt_vec, right_synonym, right_var_vec);
//    left_entity_vec->erase(std::remove_if(left_entity_vec->begin(),
//                                          left_entity_vec->end(),
//                                          [this, &right_entity_vec](Entity *entity) {
//                                            auto *stmt = dynamic_cast<Statement *>(entity);
//                                            bool has_matching_follower = false;
//                                            std::set<std::string> *follower_set =
//                                                StatementForwarder(get_normal_, stmt);
//                                            // Check if followers contain something
//                                            // from right arg vector
//                                            // remove that statement if it doesn't
//                                            for (std::string follower : *follower_set) {
//                                              if (has_two_repeated_synonyms_) {
//                                                // Adding to set if there are 2 common synonyms with
//                                                // pattern.
//                                                stmt_var_pair_vector_->push_back(
//                                                    {stmt->get_stmt_no(),
//                                                     follower});
//                                              }
//                                              for (Entity *ent : *right_entity_vec) {
//                                                std::string right_name =
//                                                    dynamic_cast<Variable *>(ent)->get_name();
//                                                if (right_name == follower) {
//                                                  has_matching_follower = true;
//                                                  break;
//                                                }
//                                              }
//                                            }
//                                            return stmt == nullptr ||
//                                                follower_set->empty() ||
//                                                !has_matching_follower;
//                                          }),
//                           left_entity_vec->end());
//    right_entity_vec->erase(std::remove_if(right_entity_vec->begin(),
//                                           right_entity_vec->end(),
//                                           [this, &left_entity_vec](Entity *entity) {
//                                             auto *variable = dynamic_cast<Variable *>(entity);
//                                             bool has_matching_followee = false;
//                                             std::set<int> *followee_set =
//                                                 VariableForwarder(get_reverse_, variable);
//                                             // Check if followees contain something
//                                             // from left arg vector
//                                             // remove that variable if it doesn't
//                                             for (int followee : *followee_set) {
//                                               for (Entity *ent : *left_entity_vec) {
//                                                 int left_num =
//                                                     dynamic_cast<Statement *>(ent)->get_stmt_no();
//                                                 if (left_num == followee) {
//                                                   has_matching_followee = true;
//                                                   break;
//                                                 }
//                                               }
//                                             }
//                                             return variable == nullptr ||
//                                                 followee_set->empty() ||
//                                                 !has_matching_followee;
//                                           }),
//                            right_entity_vec->end());
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::WildCard) {  // Uses(s, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> stmt_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));
      // Remove each statement that doesnt use anything.
      if (stmt != nullptr && !StatementForwarder(get_normal_, stmt)->empty()) {
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
      if (stmt != nullptr && StatementForwarder(get_normal_, stmt)->count(right_arg)) {
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
      if (variable != nullptr && VariableForwarder(get_reverse_, variable)->count(left_arg)) {
        var_vec.push_back(variable->get_name());
      }
    }
    ret->AddSingleColumn(right_synonym, var_vec);
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::WildCard) {  // Uses(4, _)
    int left_arg = left_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || StatementForwarder(get_normal_, stmt)->empty()) {
      // If statement with left arg as line number
      // does not use anything then return null ptr
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::Argument) {  // Uses(4, "x")
    int left_arg = left_ent.get_stmt_num();
    std::string right_arg = right_ent.get_argument();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || !StatementForwarder(get_normal_, stmt)->count(right_arg)) {
      // Return null ptr if this relationship_ is false
      return nullptr;
    }
  }
  return ret;
}


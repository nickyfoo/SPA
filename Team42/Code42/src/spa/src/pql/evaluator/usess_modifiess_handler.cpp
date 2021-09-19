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
                                     std::unordered_map<std::string, std::vector<Entity *>>
                                     *synonym_to_entity_result,
                                     SuchThatClause *relationship,
                                     std::vector<std::string> *entities_to_return) {
  this->pkb_ = pkb;
  this->synonym_to_entity_result_ = synonym_to_entity_result;
  this->relationship_ = relationship;
  this->entities_to_return_ = entities_to_return;
  this->stmt_var_pair_vector_ = nullptr;
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

void UsesSModifiesSHandler::Evaluate() {
  StmtRef left_ent = relationship_->get_left_ref()->get_stmt_ref();
  EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();

  // Going through 6 different cases for UsesS
  if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::Synonym) {  // Uses(s, v)
    std::string left_synonym = left_ent.get_synonym();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
    stmt_var_pair_vector_ = new std::vector<std::pair<int, std::string>>();
//    for (int i = 0; i < left_entity_vec->size(); i++) {
//      auto *stmt = dynamic_cast<Statement *>(left_entity_vec->at(i));
//      if (stmt == nullptr || StatementForwarder(get_normal_, stmt)->empty()) {
//        // Remove statements that do not have something it uses.
//        left_entity_vec->erase(left_entity_vec->begin() + i);
//        i--;
//      }
//    }
    left_entity_vec->erase(std::remove_if(left_entity_vec->begin(),
                                          left_entity_vec->end(),
                                          [this, &right_entity_vec](Entity *entity) {
                                            auto *stmt = dynamic_cast<Statement *>(entity);
                                            bool has_matching_follower = false;
                                            std::set<std::string> *follower_set =
                                                StatementForwarder(get_normal_, stmt);
                                            // Check if followers contain something from right arg vector
                                            // remove that statement if it doesn't
                                            //TODO: add a check here if has two repeated synonym. if there is then add to set
                                            for (std::string follower : *follower_set) {
                                              if (true) {  // TODO: change this to the has two repeated synonym check
                                                stmt_var_pair_vector_->push_back({stmt->get_stmt_no(), follower});
                                              }
                                              for (Entity *ent : *right_entity_vec) {
                                                std::string right_name = dynamic_cast<Variable *>(ent)->get_name();
                                                if (right_name == follower) {
                                                  has_matching_follower = true;
                                                  break;
                                                }
                                              }
                                            }
                                            return stmt == nullptr ||
                                                follower_set->empty() ||
                                                !has_matching_follower;
                                          }),
                           left_entity_vec->end());
//    for (int j = 0; j < right_entity_vec->size(); j++) {
//      auto *variable = dynamic_cast<Variable *>(right_entity_vec->at(j));
//      if (variable == nullptr || VariableForwarder(get_reverse_, variable)->empty()) {
//        // Remove variables that are not used
//        right_entity_vec->erase(right_entity_vec->begin() + j);
//        j--;
//      }
//    }
    right_entity_vec->erase(std::remove_if(right_entity_vec->begin(),
                                           right_entity_vec->end(),
                                           [this, &left_entity_vec](Entity *entity) {
                                             auto *variable = dynamic_cast<Variable *>(entity);
                                             bool has_matching_followee = false;
                                             std::set<int> *followee_set =
                                                 VariableForwarder(get_reverse_, variable);
                                             // Check if followees contain something from left arg vector
                                             // remove that variable if it doesn't
                                             for (int followee : *followee_set) {
                                               for (Entity *ent : *left_entity_vec) {
                                                 int left_num = dynamic_cast<Statement *>(ent)->get_stmt_no();
                                                 if (left_num == followee) {
                                                   has_matching_followee = true;
                                                   break;
                                                 }
                                               }
                                             }
                                             return variable == nullptr ||
                                                 followee_set->empty() ||
                                                 !has_matching_followee;
                                           }),
                            right_entity_vec->end());
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::WildCard) {  // Uses(s, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    for (int i = 0; i < left_entity_vec->size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec->at(i));
      // Remove each statement that doesnt use anything.
      if (stmt == nullptr || StatementForwarder(get_normal_, stmt)->empty()) {
        left_entity_vec->erase(left_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == EntRefType::Argument) {  // Uses(s, "x")
    std::string left_synonym = left_ent.get_synonym();
    std::string right_arg = right_ent.get_argument();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    for (int i = 0; i < left_entity_vec->size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec->at(i));
      // Remove each statement that doesnt have right arg in its uses
      if (stmt == nullptr || !StatementForwarder(get_normal_, stmt)->count(right_arg)) {
        left_entity_vec->erase(left_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::Synonym) {  // Uses(4, v)
    int left_arg = left_ent.get_stmt_num();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);

    Statement *stmt = pkb_->get_statement(left_arg);
    stmt->ModifiesInfo();

    for (int i = 0; i < right_entity_vec->size(); i++) {
      auto *variable = dynamic_cast<Variable *>(right_entity_vec->at(i));
      // Remove each statement that doesnt have left arg in its users.
      variable->ModifiesInfo();
      if (variable == nullptr || !VariableForwarder(get_reverse_, variable)->count(left_arg)) {
        right_entity_vec->erase(right_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::WildCard) {  // Uses(4, _)
    int left_arg = left_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || StatementForwarder(get_normal_, stmt)->empty()) {
      // If statement with left arg as line number
      // does not use anything then clear results vector
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == EntRefType::Argument) {  // Uses(4, "x")
    int left_arg = left_ent.get_stmt_num();
    std::string right_arg = right_ent.get_argument();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || !StatementForwarder(get_normal_, stmt)->count(right_arg)) {
      // Clear results vector if this relationship_ is false
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  }
}

std::vector<std::pair<int, std::string>> *UsesSModifiesSHandler::get_stmt_var_pair_vector() {
  return this->stmt_var_pair_vector_;
}

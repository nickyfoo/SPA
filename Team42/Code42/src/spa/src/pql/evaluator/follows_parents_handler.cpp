#include "follows_parents_handler.h"
#include "ast.h"

FollowsParentsHandler::FollowsParentsHandler() = default;
FollowsParentsHandler *FollowsParentsHandler::instance_ = nullptr;

FollowsParentsHandler *FollowsParentsHandler::get_instance() {
  if (!instance_) {
    instance_ = new FollowsParentsHandler();
  }
  return instance_;
}

void FollowsParentsHandler::set_function_pointers(
    std::set<int> *(Statement::*get_normal)(),
    std::set<int> *(Statement::*get_reverse)()) {
  this->get_normal_ = get_normal;
  this->get_reverse_ = get_reverse;
}

void FollowsParentsHandler::set_args(PKB *pkb,
                                     std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result,
                                     SuchThatClause relationship,
                                     std::vector<std::string> *entities_to_return,
                                     std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->synonym_to_entity_result_ = synonym_to_entity_result;
  this->relationship_ = relationship;
  this->entities_to_return_ = entities_to_return;
}

std::set<int> *FollowsParentsHandler::Forwarder(std::set<int> *(Statement::*function)(),
                                                Statement *stmt) {
  return (stmt->*function)();
}

// TODO(Sheryl): Refactor this whole part
ResultTable* FollowsParentsHandler::Evaluate() {
  ResultTable* ret = new ResultTable();
  StmtRef left_ent = relationship_.get_left_ref()->get_stmt_ref();
  StmtRef right_ent = relationship_.get_right_ref()->get_stmt_ref();
  // Going through 9 different cases for Follows/Parents
  if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::StmtNum) {  // Follows/Parents(4, 5)
    int left_arg = left_ent.get_stmt_num();
    int right_arg = right_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || !Forwarder(get_normal_, stmt)->count(right_arg)) {
      // Clear results vector if this relationship_ is false
      // or if stmt is a nullptr
//      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::StmtNum) {  // eg. Follows(s, 4)
    int right_arg = right_ent.get_stmt_num();
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);

    // Remove each statement that doesnt have right arg in its followers
    left_entity_vec->erase(std::remove_if(left_entity_vec->begin(),
                                          left_entity_vec->end(),
                                          [this, &right_arg](Entity *entity) {
                                            auto *stmt = dynamic_cast<Statement *>(entity);
                                            return stmt == nullptr ||
                                                !Forwarder(get_normal_, stmt)->count(right_arg);
                                          }),
                           left_entity_vec->end());
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::Synonym) {  // eg. Follows(4, s)
    int left_arg = left_ent.get_stmt_num();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);

    // Remove each statement that doesnt have left arg in its followees.
    right_entity_vec->erase(std::remove_if(right_entity_vec->begin(),
                                           right_entity_vec->end(),
                                           [this, &left_arg](Entity *entity) {
                                             auto *stmt = dynamic_cast<Statement *>(entity);
                                             return stmt == nullptr ||
                                                 !Forwarder(get_reverse_, stmt)->count(left_arg);
                                           }),
                            right_entity_vec->end());
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::Synonym) {  // eg Follows(s1, s2)
    std::string right_synonym = right_ent.get_synonym();
    std::string left_synonym = left_ent.get_synonym();
    if (right_synonym == left_synonym) {
      // if Follows(s1, s1), then this will always return false
      // Clear results vector and return.
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
      return;
    }

    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);

    // Remove statements that do not have a follower
    // or do not have any follower that's part of right arg vector
    left_entity_vec->erase(std::remove_if(left_entity_vec->begin(),
                                          left_entity_vec->end(),
                                          [this, &right_entity_vec](Entity *entity) {
                                            auto *stmt = dynamic_cast<Statement *>(entity);
                                            bool has_matching_follower = false;
                                            std::set<int> *follower_set =
                                                Forwarder(get_normal_, stmt);
                                            for (int follower : *follower_set) {
                                              for (Entity *ent : *right_entity_vec) {
                                                int right_num = dynamic_cast<Statement *>(ent)->get_stmt_no();
                                                if (right_num == follower) {
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
    // Remove statements that do not have a followee
    // or do not have any followee that's part of left arg vector
    right_entity_vec->erase(std::remove_if(right_entity_vec->begin(),
                                           right_entity_vec->end(),
                                           [this, &left_entity_vec](Entity *entity) {
                                             auto *stmt = dynamic_cast<Statement *>(entity);
                                             bool has_matching_followee = false;
                                             std::set<int> *followee_set =
                                                 Forwarder(get_reverse_, stmt);
                                             for (int followee : *followee_set) {
                                               for (Entity *ent : *left_entity_vec) {
                                                 int left_num = dynamic_cast<Statement *>(ent)->get_stmt_no();
                                                 if (left_num == followee) {
                                                   has_matching_followee = true;
                                                   break;
                                                 }
                                               }
                                             }
                                             return stmt == nullptr ||
                                                 followee_set->empty() ||
                                                 !has_matching_followee;
                                           }),
                            right_entity_vec->end());
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(_, _)
    std::vector<Statement *> entity_vec;
    entity_vec = pkb_->get_all_statements();
    bool found_non_empty_vec = false;
    for (int i = 0; i < entity_vec.size(); i++) {
      Statement *stmt = entity_vec.at(i);
      if (stmt == nullptr || !Forwarder(get_normal_, stmt)->empty()) {
        found_non_empty_vec = true;
        // As long as any statement has a statement following it
        // then it will return true and move on
        break;
      }
    }
    if (!found_non_empty_vec) {
      // If none of the statements have something following it
      // then clear results vector
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::StmtNum) {  // eg Follows(_, 4)
    int right_arg = right_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(right_arg);
    if (stmt == nullptr || Forwarder(get_reverse_, stmt)->empty()) {
      // If statement with right arg as line number
      // does not have any followees then clear results vector
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::Synonym) {  // eg Follows(_, s)
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);

    right_entity_vec->erase(std::remove_if(right_entity_vec->begin(),
                                           right_entity_vec->end(),
                                           [this](Entity *entity) {
                                             auto *stmt = dynamic_cast<Statement *>(entity);
                                             return stmt == nullptr ||
                                                 Forwarder(get_reverse_, stmt)->empty();
                                           }),
                            right_entity_vec->end());
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(4, _)
    int left_arg = left_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || Forwarder(get_normal_, stmt)->empty()) {
      // If statement with left arg as line number
      // does not have any followers then clear results vector
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(s, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);

    left_entity_vec->erase(std::remove_if(left_entity_vec->begin(),
                                          left_entity_vec->end(),
                                          [this](Entity *entity) {
                                            auto *stmt = dynamic_cast<Statement *>(entity);
                                            return stmt == nullptr ||
                                                Forwarder(get_normal_, stmt)->empty();
                                          }),
                           left_entity_vec->end());
  }
}


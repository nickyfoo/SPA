#include "follows_parent_handler.h"
#include <assert.h>
#include "ast.h"

FollowsParentHandler::FollowsParentHandler() {}

FollowsParentHandler *FollowsParentHandler::instance_ = nullptr;

FollowsParentHandler *FollowsParentHandler::get_instance() {
  if (!instance_) {
    instance_ = new FollowsParentHandler();
  }
  return instance_;
}

void FollowsParentHandler::set_function_pointers(
    std::set<int> *(Statement::*get_normal)(),
    std::set<int> *(Statement::*get_reverse)()) {
  this->get_normal_ = get_normal;
  this->get_reverse_ = get_reverse;
}

void FollowsParentHandler::set_args(PKB *pkb,
                                    std::shared_ptr<SuchThatClause> relationship,
                                    std::unordered_map<std::string,
                                     std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
}

std::set<int> *FollowsParentHandler::Forwarder(std::set<int> *(Statement::*function)(),
                                               Statement *stmt) {
  return (stmt->*function)();
}

// if is nullpointer, means False
// if is empty, means that True with no synonyms
ResultTable* FollowsParentHandler::Evaluate() {
  ResultTable *ret = new ResultTable();
  StmtRef left_ent = relationship_->get_left_ref()->get_stmt_ref();
  StmtRef right_ent = relationship_->get_right_ref()->get_stmt_ref();
  // Going through 9 different cases for Follows/Parents
  if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::StmtNum) {  // Follows/Parents(4, 5)
    int left_arg = left_ent.get_stmt_num();
    int right_arg = right_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || !Forwarder(get_normal_, stmt)->count(right_arg)) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::StmtNum) {  // eg. Follows(s, 4)
    int right_arg = right_ent.get_stmt_num();
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> stmt_vec;

    // Remove each statement that doesnt have right arg in its followers
    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));
      // Remove each statement that doesnt have right arg in its followers
      if (stmt != nullptr && Forwarder(get_normal_, stmt)->count(right_arg)) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
    ret->AddSingleColumn(left_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::Synonym) {  // eg. Follows(4, s)
    int left_arg = left_ent.get_stmt_num();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<std::string> stmt_vec;

    // Remove each statement that doesnt have left arg in its followees.
    for (int i = 0; i < right_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(right_entity_vec.at(i));
      // Remove each statement that doesnt have left arg in its followees,
      if (stmt != nullptr && Forwarder(get_reverse_, stmt)->count(left_arg)) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
    ret->AddSingleColumn(right_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::Synonym) {  // eg Follows(s1, s2)
    std::string right_synonym = right_ent.get_synonym();
    std::string left_synonym = left_ent.get_synonym();
    if (right_synonym == left_synonym) {
//       if Follows(s1, s1), then this will always return false
      return nullptr;
    }

    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> left_stmt_vec;
    std::vector<std::string> right_stmt_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *stmt_left = dynamic_cast<Statement *>(left_entity_vec.at(i));
      for (int j = 0; j < right_entity_vec.size(); j++) {
        auto *stmt_right = dynamic_cast<Statement *>(right_entity_vec.at(j));
        if (stmt_left != nullptr && stmt_right != nullptr
        && Forwarder(get_normal_, stmt_left)->count(stmt_right->get_stmt_no())) {
          left_stmt_vec.push_back(std::to_string(stmt_left->get_stmt_no()));
          right_stmt_vec.push_back(std::to_string(stmt_right->get_stmt_no()));
        }
      }
    }
    ret->AddDoubleColumns(left_synonym, left_stmt_vec, right_synonym, right_stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(_, _)
    std::vector<Statement *> entity_vec;
    entity_vec = pkb_->get_all_statements();
    for (int i = 0; i < entity_vec.size(); i++) {
      Statement *stmt = entity_vec.at(i);
      if (stmt != nullptr && !Forwarder(get_normal_, stmt)->empty()) {
        return ret;
      }
    }
    return nullptr;
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::StmtNum) {  // eg Follows(_, 4)
    int right_arg = right_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(right_arg);
    if (stmt == nullptr || Forwarder(get_reverse_, stmt)->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::Synonym) {  // eg Follows(_, s)
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<std::string> stmt_vec;
    for (int i = 0; i < right_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(right_entity_vec.at(i));
      if (stmt != nullptr && !Forwarder(get_reverse_, stmt)->empty()) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
    ret->AddSingleColumn(right_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(4, _)
    int left_arg = left_ent.get_stmt_num();
    Statement *stmt = pkb_->get_statement(left_arg);
    if (stmt == nullptr || Forwarder(get_normal_, stmt)->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::WildCard) {  // eg Follows(s, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> stmt_vec;
    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *stmt = dynamic_cast<Statement *>(left_entity_vec.at(i));
      if (stmt != nullptr && !Forwarder(get_normal_, stmt)->empty()) {
        stmt_vec.push_back(std::to_string(stmt->get_stmt_no()));
      }
    }
    ret->AddSingleColumn(left_synonym, stmt_vec);
  }
  return ret;
}


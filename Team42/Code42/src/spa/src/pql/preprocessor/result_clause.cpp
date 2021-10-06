#include "result_clause.h"

ResultClause::ResultClause(std::string synonym, EntityType synonym_type, ReturnType return_type) {
  this->synonym_ = synonym;
  this->synonym_type_ = synonym_type;
  this->return_type_ = return_type;
  MakeElem();
}

ResultClause::~ResultClause() = default;

std::string ResultClause::get_synonym() {
  return this->synonym_;
}

EntityType ResultClause::get_synonym_type() {
  return this->synonym_type_;
}

ReturnType ResultClause::get_return_type() {
  return this->return_type_;
}

std::string ResultClause::get_elem() {
  return this->elem_;
}

void ResultClause::MakeElem() {
  if (return_type_ == ReturnType::Default || return_type_ == ReturnType::Boolean) return;
  switch (synonym_type_) {
    case EntityType::Stmt:
      if (return_type_ == ReturnType::Integer) {
        this->elem_ = synonym_ + ".stmt#";
      }
      break;
    case EntityType::Read:
    case EntityType::Print:
      if (return_type_ == ReturnType::Integer) {
        this->elem_ = synonym_ + ".stmt#";
      } else if (return_type_ == ReturnType::Name) {
        this->elem_ = synonym_ + ".varName";
      }
      break;
    case EntityType::Call:
      if (return_type_ == ReturnType::Integer) {
        this->elem_ = synonym_ + ".stmt#";
      } else if (return_type_ == ReturnType::Name) {
        this->elem_ = synonym_ + ".procName";
      }
      break;
    case EntityType::While:
    case EntityType::If:
    case EntityType::Assign:
      if (return_type_ == ReturnType::Integer) {
        this->elem_ = synonym_ + ".stmt#";
      }
      break;
    case EntityType::Constant:
      if (return_type_ == ReturnType::Integer) {
        this->elem_ = synonym_ + ".value";
      }
      break;
    case EntityType::Variable:
      if (return_type_ == ReturnType::Name) {
        this->elem_ = synonym_ + ".varName";
      }
      break;
    case EntityType::Procedure:
      if (return_type_ == ReturnType::Name) {
        this->elem_ = synonym_ + ".procName";
      }
      break;
    default:
      this->return_type_ = ReturnType::Default;  // error
  }
}
#include <cassert>
#include <sstream>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"

IfNode::IfNode(IfWhileNodeCond *cond, std::vector<StatementNode *> then_stmt_lst,
               std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  this->cond_ = cond;
  this->then_stmt_lst_ = then_stmt_lst;
  this->else_stmt_lst_ = else_stmt_lst;
}

NodeType IfNode::get_kind() { return NodeType::If; }

Node *IfNode::get_cond() { return this->cond_; }

std::vector<StatementNode *> IfNode::get_then_stmt_lst() { return this->then_stmt_lst_; }

std::vector<StatementNode *> IfNode::get_else_stmt_lst() { return this->else_stmt_lst_; }

std::string IfNode::ToString() {
  std::stringstream res;
  res << "IfNode: {\n"
      << "Cond:\n"
      << this->cond_->ToString() + "\n"
      << "Then:";

  for (auto i = 0; i < this->then_stmt_lst_.size(); i++) {
    res << "\n#" + std::to_string(i) + ":\n" << this->then_stmt_lst_[i]->ToString();
  }

  res << "\nElse:";
  for (auto i = 0; i < this->then_stmt_lst_.size(); i++) {
    res << "\n#" + std::to_string(i) + ":\n" << this->else_stmt_lst_[i]->ToString();
  }

  res << "\nLoc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

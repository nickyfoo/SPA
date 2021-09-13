#include <cassert>
#include <stdexcept>

#include "ast.h"

IfNode::IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
               std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  if (cond == nullptr ||
      cond->get_kind() != NodeType::RelExpression && cond->get_kind() != NodeType::CondExpression) {
    throw std::invalid_argument("IfNode: expected cond to be RelExpression or CondExpression");
  }

  this->cond_ = cond;
  this->then_stmt_lst_ = then_stmt_lst;
  this->else_stmt_lst_ = else_stmt_lst;
}

NodeType IfNode::get_kind() { return NodeType::If; }

Node *IfNode::get_cond() {
  assert(this->cond_->get_kind() == NodeType::RelExpression ||
         this->cond_->get_kind() == NodeType::CondExpression);

  return this->cond_;
}

std::vector<StatementNode *> IfNode::get_then_stmt_lst() { return this->then_stmt_lst_; }

std::vector<StatementNode *> IfNode::get_else_stmt_lst() { return this->else_stmt_lst_; }


#include <cassert>
#include <stdexcept>

#include "ast.h"

IfNode::IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
               std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  if (cond == nullptr ||
      cond->kind() != NodeType::RelExpression && cond->kind() != NodeType::CondExpression) {
    throw std::invalid_argument("IfNode: expected cond to be RelExpression or CondExpression");
  }

  this->cond_ = cond;
  this->then_stmt_lst_ = then_stmt_lst;
  this->else_stmt_lst_ = else_stmt_lst;
}

NodeType IfNode::kind() { return NodeType::If; }

Node *IfNode::cond() {
  assert(this->cond_->kind() == NodeType::RelExpression ||
         this->cond_->kind() == NodeType::CondExpression);

  return this->cond_;
}

std::vector<StatementNode *> IfNode::then_stmt_lst() { return this->then_stmt_lst_; }

std::vector<StatementNode *> IfNode::else_stmt_lst() { return this->else_stmt_lst_; }

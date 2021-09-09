#include <cassert>
#include <stdexcept>

#include "ast.h"

IfNode::IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
               std::vector<StatementNode *> else_stmt_lst, int stmt_no,
               int line_no, int col_no) {
  if (cond->kind() != NodeType::RelExpression &&
      cond->kind() != NodeType::CondExpression) {
    throw std::invalid_argument(
        "IfNode: expected cond to be RelExpression or CondExpression");
  }

  this->cond_ = cond;
  this->then_stmt_lst_ = then_stmt_lst;
  this->else_stmt_lst_ = else_stmt_lst;
  this->stmt_no_ = stmt_no;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType IfNode::kind() { return NodeType::If; }

int IfNode::line_no() { return this->line_no_; }

int IfNode::col_no() { return this->col_no_; }

int IfNode::stmt_no() { return this->stmt_no_; }

Node *IfNode::cond() {
  assert(this->cond_->kind() == NodeType::RelExpression ||
         this->cond_->kind() == NodeType::CondExpression);

  return this->cond_;
}

std::vector<StatementNode *> IfNode::then_stmt_lst() {
  return this->then_stmt_lst_;
}

std::vector<StatementNode *> IfNode::else_stmt_lst() {
  return this->else_stmt_lst_;
}

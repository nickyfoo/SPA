#include <cassert>
#include <stdexcept>

#include "ast.h"

AssignNode::AssignNode(IdentifierNode *var, Node *expr, int stmt_no,
                       int line_no, int col_no) {
  if (expr->kind() != NodeType::Expression &&
      expr->kind() != NodeType::Constant &&
      expr->kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "AssignNode: expected expr to be Expression, Constant or Identifier");
  }

  this->var_ = var;
  this->expr_ = expr;
  this->stmt_no_ = stmt_no;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType AssignNode::kind() { return NodeType::Assign; }

int AssignNode::line_no() { return this->line_no_; }

int AssignNode::col_no() { return this->col_no_; }

int AssignNode::stmt_no() { return this->stmt_no_; }

IdentifierNode *AssignNode::var() { return this->var_; }

Node *AssignNode::expr() {
  assert(this->expr_->kind() == NodeType::Expression ||
         this->expr_->kind() == NodeType::Constant ||
         this->expr_->kind() == NodeType::Identifier);

  return this->expr_;
}

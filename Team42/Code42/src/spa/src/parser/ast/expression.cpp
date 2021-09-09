#include <cassert>
#include <stdexcept>

#include "ast.h"

ExpressionNode::ExpressionNode(ExprOp op, Node *left, Node *right,
                               std::string expr_string, int line_no,
                               int col_no) {
  if (left->kind() != NodeType::Expression &&
      left->kind() != NodeType::Constant &&
      left->kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "ExpressionNode: expected left to be Expression, Constant or "
        "Identifier");
  }

  if (right->kind() != NodeType::Expression &&
      right->kind() != NodeType::Constant &&
      right->kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "ExpressionNode: expected right to be Expression, Constant or "
        "Identifier");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
  this->expr_string_ = expr_string;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType ExpressionNode::kind() { return NodeType::Expression; }

int ExpressionNode::line_no() { return this->line_no_; }

int ExpressionNode::col_no() { return this->col_no_; }

ExprOp ExpressionNode::op() { return this->op_; }

Node *ExpressionNode::left() {
  assert(this->left_->kind() == NodeType::Expression ||
         this->left_->kind() == NodeType::Constant ||
         this->left_->kind() == NodeType::Identifier);

  return this->left_;
}

Node *ExpressionNode::right() {
  assert(this->right_->kind() == NodeType::Expression ||
         this->right_->kind() == NodeType::Constant ||
         this->right_->kind() == NodeType::Identifier);

  return this->right_;
}

std::string ExpressionNode::expr_string() { return this->expr_string_; }

#include <cassert>
#include <stdexcept>

#include "ast.h"

CondExpressionNode::CondExpressionNode(CondExprOp op, Node *left, Node *right,
                                       int line_no, int col_no) {
  if (left->kind() != NodeType::RelExpression &&
      left->kind() != NodeType::CondExpression) {
    throw std::invalid_argument(
        "CondExpressionNode: expected left to be RelExpression or "
        "CondExpression");
  }

  if (right->kind() != NodeType::RelExpression &&
      right->kind() != NodeType::CondExpression) {
    throw std::invalid_argument(
        "CondExpressionNode: expected right to be RelExpression or "
        "CondExpression");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType CondExpressionNode::kind() { return NodeType::CondExpression; }

int CondExpressionNode::line_no() { return this->line_no_; }

int CondExpressionNode::col_no() { return this->col_no_; }

CondExprOp CondExpressionNode::op() { return this->op_; }

Node *CondExpressionNode::left() {
  assert(this->left_->kind() == NodeType::RelExpression ||
         this->left_->kind() == NodeType::CondExpression);

  return this->left_;
}

Node *CondExpressionNode::right() {
  assert(this->right_->kind() == NodeType::RelExpression ||
         this->right_->kind() == NodeType::CondExpression);

  return this->right_;
}

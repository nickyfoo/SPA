#include <cassert>
#include <stdexcept>

#include "ast.h"

CondExpressionNode::CondExpressionNode(CondExprOp op, Node *left, Node *right, LocInfo loc)
    : Node(loc) {
  if (left == nullptr ||
      left->kind() != NodeType::RelExpression && left->kind() != NodeType::CondExpression) {
    throw std::invalid_argument(
        "CondExpressionNode: expected left to be RelExpression or "
        "CondExpression");
  }

  if (op == CondExprOp::Not && right != nullptr) {
    throw std::invalid_argument("CondExpressionNode: (Not) expected right to be nullptr");
  }

  if (op != CondExprOp::Not &&
      (right == nullptr ||
       right->kind() != NodeType::RelExpression && right->kind() != NodeType::CondExpression)) {
    throw std::invalid_argument(
        "CondExpressionNode: expected right to be RelExpression or "
        "CondExpression");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
}

NodeType CondExpressionNode::kind() { return NodeType::CondExpression; }

CondExprOp CondExpressionNode::op() { return this->op_; }

Node *CondExpressionNode::left() {
  assert(this->left_->kind() == NodeType::RelExpression ||
         this->left_->kind() == NodeType::CondExpression);

  return this->left_;
}

Node *CondExpressionNode::right() {
  if (this->op_ == CondExprOp::Not) {
    assert(this->right_ == nullptr);
    return nullptr;
  }

  assert(this->right_->kind() == NodeType::RelExpression ||
         this->right_->kind() == NodeType::CondExpression);

  return this->right_;
}

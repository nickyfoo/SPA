#include <cassert>
#include <stdexcept>

#include "ast.h"

RelExpressionNode::RelExpressionNode(RelExprOp op, Node *left, Node *right, LocInfo loc)
    : Node(loc) {
  if (left == nullptr || left->kind() != NodeType::Expression &&
                             left->kind() != NodeType::Constant &&
                             left->kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "RelExpressionNode: expected left to be Expression, Constant or "
        "Identifier");
  }

  if (right == nullptr || right->kind() != NodeType::Expression &&
                              right->kind() != NodeType::Constant &&
                              right->kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "RelExpressionNode: expected right to be Expression, Constant or "
        "Identifier");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
}

NodeType RelExpressionNode::kind() { return NodeType::RelExpression; }

RelExprOp RelExpressionNode::op() { return this->op_; }

Node *RelExpressionNode::left() {
  assert(this->left_->kind() == NodeType::Expression || this->left_->kind() == NodeType::Constant ||
         this->left_->kind() == NodeType::Identifier);

  return this->left_;
}

Node *RelExpressionNode::right() {
  assert(this->right_->kind() == NodeType::Expression ||
         this->right_->kind() == NodeType::Constant ||
         this->right_->kind() == NodeType::Identifier);

  return this->right_;
}

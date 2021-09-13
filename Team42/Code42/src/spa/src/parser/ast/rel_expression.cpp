#include <cassert>
#include <stdexcept>

#include "ast.h"

RelExpressionNode::RelExpressionNode(RelExprOp op, Node *left, Node *right, LocInfo loc)
    : Node(loc) {
  if (left == nullptr || left->get_kind() != NodeType::Expression &&
                             left->get_kind() != NodeType::Constant &&
                             left->get_kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "RelExpressionNode: expected left to be Expression, Constant or "
        "Identifier");
  }

  if (right == nullptr || right->get_kind() != NodeType::Expression &&
                              right->get_kind() != NodeType::Constant &&
                              right->get_kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "RelExpressionNode: expected right to be Expression, Constant or "
        "Identifier");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
}

NodeType RelExpressionNode::get_kind() { return NodeType::RelExpression; }

RelExprOp RelExpressionNode::get_op() { return this->op_; }

Node *RelExpressionNode::get_left() {
  assert(this->left_->get_kind() == NodeType::Expression || this->left_->get_kind() == NodeType::Constant ||
         this->left_->get_kind() == NodeType::Identifier);

  return this->left_;
}

Node *RelExpressionNode::get_right() {
  assert(this->right_->get_kind() == NodeType::Expression ||
         this->right_->get_kind() == NodeType::Constant ||
         this->right_->get_kind() == NodeType::Identifier);

  return this->right_;
}


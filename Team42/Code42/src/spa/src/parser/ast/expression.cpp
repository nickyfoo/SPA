#include <cassert>
#include <stdexcept>

#include "ast.h"

ExpressionNode::ExpressionNode(ExprOp op, Node *left, Node *right, std::string expr_string,
                               LocInfo loc)
    : Node(loc) {
  if (left == nullptr || left->get_kind() != NodeType::Expression &&
                             left->get_kind() != NodeType::Constant &&
                             left->get_kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "ExpressionNode: expected left to be Expression, Constant or "
        "Identifier");
  }

  if (right == nullptr || right->get_kind() != NodeType::Expression &&
                              right->get_kind() != NodeType::Constant &&
                              right->get_kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "ExpressionNode: expected right to be Expression, Constant or "
        "Identifier");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
  this->expr_string_ = expr_string;
}

NodeType ExpressionNode::get_kind() { return NodeType::Expression; }

ExprOp ExpressionNode::get_op() { return this->op_; }

Node *ExpressionNode::get_left() {
  assert(this->left_ != nullptr);
  assert(this->left_->get_kind() == NodeType::Expression || this->left_->get_kind() == NodeType::Constant ||
         this->left_->get_kind() == NodeType::Identifier);

  return this->left_;
}

Node *ExpressionNode::get_right() {
  assert(this->right_ != nullptr);
  assert(this->right_->get_kind() == NodeType::Expression ||
         this->right_->get_kind() == NodeType::Constant ||
         this->right_->get_kind() == NodeType::Identifier);

  return this->right_;
}

std::string ExpressionNode::get_expr_string() { return this->expr_string_; }


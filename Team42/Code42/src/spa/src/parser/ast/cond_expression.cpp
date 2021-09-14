#include <cassert>
#include <stdexcept>

#include "ast.h"

CondExpressionNode::CondExpressionNode(CondExprOp op, Node *left, Node *right,
                                       std::string expr_string, LocInfo loc)
    : Node(loc) {
  if (left == nullptr ||
      left->get_kind() != NodeType::RelExpression && left->get_kind() != NodeType::CondExpression) {
    throw std::invalid_argument(
        "CondExpressionNode: expected left to be RelExpression or "
        "CondExpression");
  }

  if (op == CondExprOp::Not && right != nullptr) {
    throw std::invalid_argument("CondExpressionNode: (Not) expected right to be nullptr");
  }

  if (op != CondExprOp::Not &&
      (right == nullptr || right->get_kind() != NodeType::RelExpression &&
                               right->get_kind() != NodeType::CondExpression)) {
    throw std::invalid_argument(
        "CondExpressionNode: expected right to be RelExpression or "
        "CondExpression");
  }

  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
  this->expr_string_ = expr_string;
}

NodeType CondExpressionNode::get_kind() { return NodeType::CondExpression; }

CondExprOp CondExpressionNode::get_op() { return this->op_; }

Node *CondExpressionNode::get_left() {
  assert(this->left_->get_kind() == NodeType::RelExpression ||
         this->left_->get_kind() == NodeType::CondExpression);

  return this->left_;
}

Node *CondExpressionNode::get_right() {
  if (this->op_ == CondExprOp::Not) {
    assert(this->right_ == nullptr);
    return nullptr;
  }

  assert(this->right_->get_kind() == NodeType::RelExpression ||
         this->right_->get_kind() == NodeType::CondExpression);

  return this->right_;
}

std::string CondExpressionNode::get_expr_string() { return this->expr_string_; }

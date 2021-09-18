#include <cassert>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"
#include "string_utils.h"

ExpressionNode::ExpressionNode(ExprOp op, ExpressionNodeChild *left, ExpressionNodeChild *right,
                               LocInfo loc)
    : Node(loc) {
  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
}

NodeType ExpressionNode::get_kind() { return NodeType::Expression; }

ExprOp ExpressionNode::get_op() { return this->op_; }

Node *ExpressionNode::get_left() { return this->left_; }

Node *ExpressionNode::get_right() { return this->right_; }

std::string ExpressionNode::get_expr_string() {
  std::string op, left, right;

  op = ExprOpToString(this->op_);
  if (this->left_) {
    left = this->left_->get_expr_string();
  }
  if (this->right_) {
    right = this->right_->get_expr_string();
  }

  return StringFormat("%s %s %s", left.c_str(), right.c_str(), op.c_str());
}

#include <cassert>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"
#include "string_utils.h"

CondExpressionNode::CondExpressionNode(CondExprOp op, CondExpressionNodeChild *left,
                                       CondExpressionNodeChild *right, LocInfo loc)
    : Node(loc) {
  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
}

NodeType CondExpressionNode::get_kind() { return NodeType::CondExpression; }

CondExprOp CondExpressionNode::get_op() { return this->op_; }

Node *CondExpressionNode::get_left() { return this->left_; }

Node *CondExpressionNode::get_right() { return this->right_; }

std::string CondExpressionNode::get_expr_string() {
  std::string op, left, right;

  op = CondExprOpToString(this->op_);
  if (this->left_) {
    left = this->left_->get_expr_string();
  }
  if (this->right_) {
    right = this->right_->get_expr_string();
  }

  return StringFormat("%s %s %s", left.c_str(), right.c_str(), op.c_str());
}

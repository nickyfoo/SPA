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
  std::string expr_string = ExprOpToString(this->op_);
  if (this->right_) {
    expr_string =
        StringFormat("%s %s", this->right_->get_expr_string().c_str(), expr_string.c_str());
  }

  if (this->left_) {
    expr_string =
        StringFormat("%s %s", this->left_->get_expr_string().c_str(), expr_string.c_str());
  }

  return expr_string;
}

std::string ExpressionNode::ToString() {
  std::string op, left, right;

  op = ExprOpToString(this->op_);
  if (this->left_) {
    left = this->left_->ToString();
  }
  if (this->right_) {
    right = this->right_->ToString();
  }

  return StringFormat("ExpressionNode: {\nOp: %s\nLeft:\n%s\nRight:\n%s\nLine: %d\nCol: %d\n}",
                      op.c_str(), left.c_str(), right.c_str(), this->get_line_no(),
                      this->get_col_no());
}

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
  std::string expr_string = CondExprOpToString(this->op_);
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

std::string CondExpressionNode::ToString() {
  std::string op, left, right;

  op = CondExprOpToString(this->op_);
  if (this->left_) {
    left = this->left_->ToString();
  }
  if (this->right_) {
    right = this->right_->ToString();
  }
  return StringFormat("CondExpresionNode: {\nOp: %s\nLeft:\n%s\nRight:\n%s\nLine: %d\nCol: %d\n}",
                      op.c_str(), left.c_str(), right.c_str(), this->get_line_no(),
                      this->get_col_no());
}

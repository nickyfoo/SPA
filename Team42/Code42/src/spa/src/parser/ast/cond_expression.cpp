#include <cassert>
#include <sstream>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"

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
    expr_string = this->right_->get_expr_string() + " " + expr_string;
  }

  if (this->left_) {
    expr_string = this->left_->get_expr_string() + " " + expr_string;
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

  std::stringstream res;
  res << "CondExpressionNode: {\n"
      << "Op: " + op + "\n"
      << "Left:\n"
      << left + "\n"
      << "Right:\n"
      << right + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

#include <cassert>
#include <stdexcept>
#include <sstream>

#include "ast.h"
#include "ast_utils.hpp"

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
    expr_string = this->right_->get_expr_string() + " " + expr_string;
  }

  if (this->left_) {
    expr_string = this->left_->get_expr_string() + " " + expr_string;
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

  std::stringstream res;
  res << "ExpressionNode: {\n"
      << "Op: " + op + "\n"
      << "Left:\n" 
      << left + "\n"
      << "Right:\n"
      << right + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

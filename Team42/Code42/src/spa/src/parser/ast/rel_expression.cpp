#include <cassert>
#include <sstream>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"

RelExpressionNode::RelExpressionNode(RelExprOp op, RelExpressionNodeChild *left,
                                     RelExpressionNodeChild *right, LocInfo loc)
    : Node(loc) {
  this->op_ = op;
  this->left_ = left;
  this->right_ = right;
}

NodeType RelExpressionNode::get_kind() { return NodeType::RelExpression; }

RelExprOp RelExpressionNode::get_op() { return this->op_; }

Node *RelExpressionNode::get_left() { return this->left_; }

Node *RelExpressionNode::get_right() { return this->right_; }

std::string RelExpressionNode::get_expr_string() {
  std::string expr_string = RelExprOpToString(this->op_);
  if (this->right_) {
    expr_string = this->right_->get_expr_string() + " " + expr_string;
  }

  if (this->left_) {
    expr_string = this->left_->get_expr_string() + " " + expr_string;
  }

  return expr_string;
}

std::string RelExpressionNode::ToString() {
  std::string op, left, right;

  op = RelExprOpToString(this->op_);
  if (this->left_) {
    left = this->left_->ToString();
  }
  if (this->right_) {
    right = this->right_->ToString();
  }

  std::stringstream res;
  res << "RelExpressionNode: {\n"
      << "Op: " + op + "\n"
      << "Left:\n"
      << left + "\n"
      << "Right:\n"
      << right + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

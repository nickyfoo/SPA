#include <cassert>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"
#include "string_utils.h"

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
    expr_string =
        StringFormat("%s %s", this->right_->get_expr_string().c_str(), expr_string.c_str());
  }

  if (this->left_) {
    expr_string =
        StringFormat("%s %s", this->left_->get_expr_string().c_str(), expr_string.c_str());
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

  return StringFormat("RelExpressionNode: {\nOp: %s\nLeft:\n%s\nRight:\n%s\n}", op.c_str(),
                      left.c_str(), right.c_str());
}

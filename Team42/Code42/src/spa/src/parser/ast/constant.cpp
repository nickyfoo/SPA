#include "ast.h"

ConstantNode::ConstantNode(std::string value, int line_no, int col_no) {
  this->value_ = value;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType ConstantNode::kind() { return NodeType::Constant; }

int ConstantNode::line_no() { return this->line_no_; }

int ConstantNode::col_no() { return this->col_no_; }

std::string ConstantNode::value() { return this->value_; }

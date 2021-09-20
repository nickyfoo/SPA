#include "ast.h"
#include "ast_utils.hpp"

#include <sstream>

ConstantNode::ConstantNode(std::string value, LocInfo loc) : Node(loc) { this->value_ = value; }

NodeType ConstantNode::get_kind() { return NodeType::Constant; };

std::string ConstantNode::get_value() { return this->value_; }

std::string ConstantNode::get_expr_string() { return this->value_; }

std::string ConstantNode::ToString() {
  std::stringstream res;
  res << "ConstantNode: {\n"
      << "Val:\n"
      << this->value_ + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";
  return res.str();
}

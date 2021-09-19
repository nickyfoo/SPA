#include <sstream>
#include <string>

#include "ast.h"
#include "ast_utils.hpp"

IdentifierNode::IdentifierNode(std::string name, LocInfo loc) : Node(loc) { this->name_ = name; }

NodeType IdentifierNode::get_kind() { return NodeType::Identifier; }

std::string IdentifierNode::get_name() { return this->name_; }

std::string IdentifierNode::get_expr_string() { return this->name_; }

std::string IdentifierNode::ToString() {
  std::stringstream res;
  res << "IdentifierNode: {\n"
      << "Name: " + this->name_ + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";
  return res.str();
}

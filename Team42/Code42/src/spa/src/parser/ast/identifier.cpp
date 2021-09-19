#include <string>

#include "ast.h"
#include "string_utils.h"

IdentifierNode::IdentifierNode(std::string name, LocInfo loc) : Node(loc) { this->name_ = name; }

NodeType IdentifierNode::get_kind() { return NodeType::Identifier; }

std::string IdentifierNode::get_name() { return this->name_; }

std::string IdentifierNode::get_expr_string() { return this->name_; }

std::string IdentifierNode::ToString() {
  return StringFormat("IdentifierNode: {\nName: %s\nLine: %d\nCol: %d\n}", this->name_.c_str(),
                      this->get_line_no(), this->get_col_no());
}

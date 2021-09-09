#include <string>

#include "ast.h"

IdentifierNode::IdentifierNode(std::string name, int line_no, int col_no) {
  this->name_ = name;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType IdentifierNode::kind() { return NodeType::Identifier; }

int IdentifierNode::line_no() { return this->line_no_; }

int IdentifierNode::col_no() { return this->col_no_; }

std::string IdentifierNode::name() { return this->name_; }

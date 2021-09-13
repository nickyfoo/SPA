#include <string>

#include "ast.h"

IdentifierNode::IdentifierNode(std::string name, LocInfo loc) : Node(loc) { this->name_ = name; }

NodeType IdentifierNode::get_kind() { return NodeType::Identifier; }

std::string IdentifierNode::get_name() { return this->name_; }

#include <string>

#include "ast.h"

IdentifierNode::IdentifierNode(std::string name, LocInfo loc) : Node(loc) { this->name_ = name; }

NodeType IdentifierNode::kind() { return NodeType::Identifier; }

std::string IdentifierNode::name() { return this->name_; }

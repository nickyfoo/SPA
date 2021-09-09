#include "ast.h"

ConstantNode::ConstantNode(std::string value, LocInfo loc) : Node(loc) { this->value_ = value; }

NodeType ConstantNode::kind() { return NodeType::Constant; };

std::string ConstantNode::value() { return this->value_; }

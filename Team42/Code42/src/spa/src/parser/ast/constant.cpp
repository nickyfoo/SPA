#include "ast.h"

ConstantNode::ConstantNode(std::string value, LocInfo loc) : Node(loc) { this->value_ = value; }

NodeType ConstantNode::get_kind() { return NodeType::Constant; };

std::string ConstantNode::get_value() { return this->value_; }

std::string ConstantNode::get_expr_string() { return this->value_; }


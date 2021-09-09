#include "ast.h"

ReadNode::ReadNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType ReadNode::kind() { return NodeType::Read; }

IdentifierNode *ReadNode::var() { return this->var_; }

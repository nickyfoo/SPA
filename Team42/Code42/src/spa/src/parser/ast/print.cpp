#include "ast.h"

PrintNode::PrintNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType PrintNode::get_kind() { return NodeType::Print; }

IdentifierNode *PrintNode::get_var() { return this->var_; }


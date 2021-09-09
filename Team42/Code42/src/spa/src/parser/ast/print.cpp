#include "ast.h"

PrintNode::PrintNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType PrintNode::kind() { return NodeType::Print; }

IdentifierNode *PrintNode::var() { return this->var_; }

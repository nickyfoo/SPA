#include "ast.h"
#include "string_utils.h"

ReadNode::ReadNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType ReadNode::get_kind() { return NodeType::Read; }

IdentifierNode *ReadNode::get_var() { return this->var_; }

std::string ReadNode::ToString() {
  return StringFormat("ReadNode: {\nVar:\n%s\n}", this->var_->ToString().c_str());
}

#include "ast.h"
#include "string_utils.h"

PrintNode::PrintNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType PrintNode::get_kind() { return NodeType::Print; }

IdentifierNode *PrintNode::get_var() { return this->var_; }

std::string PrintNode::ToString() {
  return StringFormat("PrintNode: {\nVar:\n%s\n}", this->var_->ToString().c_str());
}

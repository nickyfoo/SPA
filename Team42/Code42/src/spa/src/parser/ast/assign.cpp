#include <cassert>
#include <stdexcept>

#include "ast.h"
#include "string_utils.h"

AssignNode::AssignNode(IdentifierNode *var, AssignNodeExpr *expr, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  this->var_ = var;
  this->expr_ = expr;
}

NodeType AssignNode::get_kind() { return NodeType::Assign; }

IdentifierNode *AssignNode::get_var() { return this->var_; }

Node *AssignNode::expr() { return this->expr_; }

std::string AssignNode::ToString() {
  return StringFormat("AssignNode: {\nVar:\n%s\nExpr:\n%s\n}", this->var_->ToString().c_str(),
                      this->expr_->ToString().c_str());
}

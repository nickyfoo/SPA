#include <cassert>
#include <stdexcept>

#include "ast.h"

AssignNode::AssignNode(IdentifierNode *var, Node *expr, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  if (var == nullptr) {
    throw std::invalid_argument("AssignNode: expected var to be Identifier");
  }

  if (expr == nullptr || expr->kind() != NodeType::Expression &&
                             expr->kind() != NodeType::Constant &&
                             expr->kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "AssignNode: expected expr to be Expression, Constant or Identifier");
  }

  this->var_ = var;
  this->expr_ = expr;
}

NodeType AssignNode::kind() { return NodeType::Assign; }

IdentifierNode *AssignNode::var() { return this->var_; }

Node *AssignNode::expr() {
  assert(this->expr_->kind() == NodeType::Expression || this->expr_->kind() == NodeType::Constant ||
         this->expr_->kind() == NodeType::Identifier);

  return this->expr_;
}

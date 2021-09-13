#include <cassert>
#include <stdexcept>

#include "ast.h"

AssignNode::AssignNode(IdentifierNode *var, Node *expr, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  if (var == nullptr) {
    throw std::invalid_argument("AssignNode: expected var to be Identifier");
  }

  if (expr == nullptr || expr->get_kind() != NodeType::Expression &&
                             expr->get_kind() != NodeType::Constant &&
                             expr->get_kind() != NodeType::Identifier) {
    throw std::invalid_argument(
        "AssignNode: expected expr to be Expression, Constant or Identifier");
  }

  this->var_ = var;
  this->expr_ = expr;
}

NodeType AssignNode::get_kind() { return NodeType::Assign; }

IdentifierNode *AssignNode::get_var() { return this->var_; }

Node *AssignNode::expr() {
  assert(this->expr_->get_kind() == NodeType::Expression || this->expr_->get_kind() == NodeType::Constant ||
         this->expr_->get_kind() == NodeType::Identifier);

  return this->expr_;
}

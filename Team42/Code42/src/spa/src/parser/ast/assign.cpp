#include <cassert>
#include <sstream>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"

AssignNode::AssignNode(IdentifierNode *var, AssignNodeExpr *expr, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  this->var_ = var;
  this->expr_ = expr;
}

NodeType AssignNode::get_kind() { return NodeType::Assign; }

IdentifierNode *AssignNode::get_var() { return this->var_; }

Node *AssignNode::expr() { return this->expr_; }

std::string AssignNode::ToString() {
  std::stringstream res;
  res << "AssignNode: {\n"
      << "Var:\n"
      << this->var_->ToString() + "\n"
      << "Expr:\n"
      << this->expr_->ToString() + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

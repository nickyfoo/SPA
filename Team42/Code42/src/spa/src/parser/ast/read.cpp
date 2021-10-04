#include <sstream>

#include "ast.h"
#include "ast_utils.hpp"

ReadNode::ReadNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType ReadNode::get_kind() { return NodeType::Read; }

IdentifierNode *ReadNode::get_var() { return this->var_; }

std::string ReadNode::ToString() {
  std::stringstream res;
  res << "ReadNode: {\n"
      << "Var:\n"
      << this->var_->ToString() + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

#include <sstream>

#include "ast.h"
#include "ast_utils.hpp"

PrintNode::PrintNode(IdentifierNode *var, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->var_ = var;
}

NodeType PrintNode::get_kind() { return NodeType::Print; }

IdentifierNode *PrintNode::get_var() { return this->var_; }

std::string PrintNode::ToString() {
  std::stringstream res;
  res << "PrintNode: {\n"
      << "Var:\n"
      << this->var_->ToString() + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

#include <sstream>

#include "ast.h"
#include "ast_utils.hpp"

CallNode::CallNode(IdentifierNode *proc, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->proc_ = proc;
}

NodeType CallNode::get_kind() { return NodeType::Call; }

IdentifierNode *CallNode::get_proc() { return this->proc_; }

std::string CallNode::ToString() {
  std::stringstream res;
  res << "CallNode: {\n"
      << "Proc:\n"
      << this->proc_->ToString() + "\n"
      << "Loc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";
  return res.str();
}

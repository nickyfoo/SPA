#include "ast.h"
#include "string_utils.h"

CallNode::CallNode(IdentifierNode *proc, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->proc_ = proc;
}

NodeType CallNode::get_kind() { return NodeType::Call; }

IdentifierNode *CallNode::get_proc() { return this->proc_; }

std::string CallNode::ToString() {
  return StringFormat("CallNode: {\nProc:\n%s\nExpr:\n%s\nLine: %d\nCol: %d\n}",
                      this->proc_->ToString().c_str(), this->get_line_no(), this->get_col_no());
}

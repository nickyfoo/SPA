#include "ast.h"

CallNode::CallNode(IdentifierNode *proc, int stmt_no, LocInfo loc) : StatementNode(stmt_no, loc) {
  this->proc_ = proc;
}

NodeType CallNode::kind() { return NodeType::Call; }

IdentifierNode *CallNode::proc() { return this->proc_; }

#include "ast.h"

CallNode::CallNode(IdentifierNode *proc, int stmt_no, int line_no, int col_no) {
  this->proc_ = proc;
  this->stmt_no_ = stmt_no;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType CallNode::kind() { return NodeType::Call; }

int CallNode::line_no() { return this->line_no_; }

int CallNode::col_no() { return this->col_no_; }

int CallNode::stmt_no() { return this->stmt_no_; }

IdentifierNode *CallNode::proc() { return this->proc_; }

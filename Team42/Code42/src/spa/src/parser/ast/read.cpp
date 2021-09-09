#include "ast.h"

ReadNode::ReadNode(IdentifierNode *var, int stmt_no, int line_no, int col_no) {
  this->var_ = var;
  this->stmt_no_ = stmt_no;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType ReadNode::kind() { return NodeType::Read; }

int ReadNode::line_no() { return this->line_no_; }

int ReadNode::col_no() { return this->col_no_; }

int ReadNode::stmt_no() { return this->stmt_no_; }

IdentifierNode *ReadNode::var() { return this->var_; }

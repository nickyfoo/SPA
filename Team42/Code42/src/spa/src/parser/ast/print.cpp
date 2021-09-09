#include "ast.h"

PrintNode::PrintNode(IdentifierNode *var, int stmt_no, int line_no,
                     int col_no) {
  this->var_ = var;
  this->stmt_no_ = stmt_no;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType PrintNode::kind() { return NodeType::Print; }

int PrintNode::line_no() { return this->line_no_; }

int PrintNode::col_no() { return this->col_no_; }

int PrintNode::stmt_no() { return this->stmt_no_; }

IdentifierNode *PrintNode::var() { return this->var_; }

#include "ast.h"

Node::Node(LocInfo loc) {
  this->line_no_ = loc.line_no;
  this->col_no_ = loc.col_no;
}

int Node::get_line_no() { return this->line_no_; }

int Node::get_col_no() { return this->col_no_; }


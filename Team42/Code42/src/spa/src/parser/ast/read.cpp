#include "ast.h"

ReadNode::ReadNode(IdentifierNode *var, int stmt_no, int line_no, int col_no) {}
NodeType ReadNode::kind() {}
int ReadNode::line_no() {}
int ReadNode::col_no() {}
int ReadNode::stmt_no() {}
IdentifierNode ReadNode::*var() {}

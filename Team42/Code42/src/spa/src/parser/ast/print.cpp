#include "ast.h"

PrintNode::PrintNode(IdentifierNode *var, int stmt_no, int line_no,
                     int col_no) {}
NodeType PrintNode::kind() {}
int PrintNode::line_no() {}
int PrintNode::col_no() {}
int PrintNode::stmt_no() {}
IdentifierNode PrintNode::*var() {}

#include "ast.h"

AssignNode::AssignNode(IdentifierNode *var, ExpressionNode *expr, int stmt_no,
                       int line_no, int col_no) {}
NodeType AssignNode::kind() {}
int AssignNode::line_no() {}
int AssignNode::col_no() {}
int AssignNode::stmt_no() {}
IdentifierNode AssignNode::var() {}
Node AssignNode::*expr() {}

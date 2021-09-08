#include "ast.h"

CondExpressionNode::CondExpressionNode(CondExprOp op, Node *left, Node *right,
                                       int line_no, int col_no) {}
NodeType CondExpressionNode::kind() {}
int CondExpressionNode::line_no() {}
int CondExpressionNode::col_no() {}
CondExprOp CondExpressionNode::op() {}
Node CondExpressionNode::*left() {}
Node CondExpressionNode::*right() {}

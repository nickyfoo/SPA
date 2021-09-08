#include "ast.h"

ExpressionNode::ExpressionNode(ExprOp op, Node *left, Node *right,
                               std::string expr_string, int line_no,
                               int col_no) {}
NodeType ExpressionNode::kind() {}
int ExpressionNode::line_no() {}
int ExpressionNode::col_no() {}
ExprOp ExpressionNode::op() {}
Node ExpressionNode::*left() {}
Node ExpressionNode::*right() {}
std::string ExpressionNode::exprString() {}

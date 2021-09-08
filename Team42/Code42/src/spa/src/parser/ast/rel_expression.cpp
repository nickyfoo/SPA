#include "ast.h"

RelExpressionNode::RelExpressionNode(RelExprOp op, Node *left, Node *right,
                                     int lineNo, int colNo) {}
NodeType RelExpressionNode::kind() {}
int RelExpressionNode::line_no() {}
int RelExpressionNode::col_no() {}
RelExprOp RelExpressionNode::op() {}
Node RelExpressionNode::*left() {}
Node RelExpressionNode::*right() {}

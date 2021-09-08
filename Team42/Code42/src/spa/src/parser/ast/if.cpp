#include "ast.h"

IfNode::IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
               std::vector<StatementNode *> else_stmt_lst, int stmt_no,
               int line_no, int col_no) {}
NodeType IfNode::kind() {}
int IfNode::line_no() {}
int IfNode::col_no() {}
int IfNode::stmt_no() {}
Node IfNode::*cond() {}
std::vector<StatementNode *> IfNode::then_stmt_lst() {}
std::vector<StatementNode *> IfNode::else_stmt_lst() {}

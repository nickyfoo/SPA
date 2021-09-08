#include "ast.h"

WhileNode::WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst,
                     int stmt_no, int line_no, int col_no) {}
NodeType WhileNode::kind() {}
int WhileNode::line_no() {}
int WhileNode::col_no() {}
int WhileNode::stmt_no() {}
Node WhileNode::*cond() {}
std::vector<StatementNode *> WhileNode::stmt_list() {}

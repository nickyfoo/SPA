#include <cassert>
#include <stdexcept>

#include "ast.h"

WhileNode::WhileNode(IfWhileNodeCond *cond, std::vector<StatementNode *> stmt_lst, int stmt_no,
                     LocInfo loc)
    : StatementNode(stmt_no, loc) {
  this->cond_ = cond;
  this->stmt_lst_ = stmt_lst;
}

NodeType WhileNode::get_kind() { return NodeType::While; }

Node *WhileNode::get_cond() { return this->cond_; }

std::vector<StatementNode *> WhileNode::get_stmt_list() { return this->stmt_lst_; }

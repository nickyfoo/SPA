#include <cassert>
#include <stdexcept>

#include "ast.h"

WhileNode::WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  if (cond == nullptr ||
      cond->kind() != NodeType::RelExpression && cond->kind() != NodeType::CondExpression) {
    throw std::invalid_argument("WhileNode: expected cond to be RelExpression or CondExpression");
  }

  this->cond_ = cond;
  this->stmt_lst_ = stmt_lst;
}

NodeType WhileNode::kind() { return NodeType::While; }

Node *WhileNode::cond() {
  assert(this->cond_->kind() == NodeType::RelExpression ||
         this->cond_->kind() == NodeType::CondExpression);

  return this->cond_;
}

std::vector<StatementNode *> WhileNode::stmt_list() { return this->stmt_lst_; }

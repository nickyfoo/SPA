#include <cassert>
#include <stdexcept>

#include "ast.h"

WhileNode::WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  if (cond == nullptr ||
      cond->get_kind() != NodeType::RelExpression && cond->get_kind() != NodeType::CondExpression) {
    throw std::invalid_argument("WhileNode: expected cond to be RelExpression or CondExpression");
  }

  this->cond_ = cond;
  this->stmt_lst_ = stmt_lst;
}

NodeType WhileNode::get_kind() { return NodeType::While; }

Node *WhileNode::get_cond() {
  assert(this->cond_->get_kind() == NodeType::RelExpression ||
         this->cond_->get_kind() == NodeType::CondExpression);

  return this->cond_;
}

std::vector<StatementNode *> WhileNode::get_stmt_list() { return this->stmt_lst_; }

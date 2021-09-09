#include <cassert>
#include <stdexcept>

#include "ast.h"

WhileNode::WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst,
                     int stmt_no, int line_no, int col_no) {
  if (cond->kind() != NodeType::RelExpression ||
      cond->kind() != NodeType::CondExpression) {
    throw std::invalid_argument(
        "WhileNode: expected cond to be RelExpression or CondExpression");
  }

  this->cond_ = cond;
  this->stmt_lst_ = stmt_lst;
  this->stmt_no_ = stmt_no;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType WhileNode::kind() { return NodeType::While; }

int WhileNode::line_no() { return this->line_no_; }

int WhileNode::col_no() { return this->col_no_; }

int WhileNode::stmt_no() { return this->stmt_no_; }

Node *WhileNode::cond() {
  assert(this->cond_->kind() == NodeType::RelExpression ||
         this->cond_->kind() == NodeType::CondExpression);

  return this->cond_;
}

std::vector<StatementNode *> WhileNode::stmt_list() { return this->stmt_lst_; }

#include <cassert>
#include <stdexcept>

#include "ast.h"
#include "string_utils.h"

IfNode::IfNode(IfWhileNodeCond *cond, std::vector<StatementNode *> then_stmt_lst,
               std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc)
    : StatementNode(stmt_no, loc) {
  this->cond_ = cond;
  this->then_stmt_lst_ = then_stmt_lst;
  this->else_stmt_lst_ = else_stmt_lst;
}

NodeType IfNode::get_kind() { return NodeType::If; }

Node *IfNode::get_cond() { return this->cond_; }

std::vector<StatementNode *> IfNode::get_then_stmt_lst() { return this->then_stmt_lst_; }

std::vector<StatementNode *> IfNode::get_else_stmt_lst() { return this->else_stmt_lst_; }

std::string IfNode::ToString() {
  std::string res = StringFormat("IfNode: {\nCond:\n%s\nThen:", this->cond_->ToString().c_str());
  for (auto i = 0; i < this->then_stmt_lst_.size(); i++) {
    res += StringFormat("\n#%d:\n%s", i + 1, this->then_stmt_lst_[i]->ToString().c_str());
  }

  res += "\nElse:";
  for (auto i = 0; i < this->else_stmt_lst_.size(); i++) {
    res += StringFormat("\n#%d:\n%s", i + 1, this->else_stmt_lst_[i]->ToString().c_str());
  }

  res += "\n}";

  return res;
}

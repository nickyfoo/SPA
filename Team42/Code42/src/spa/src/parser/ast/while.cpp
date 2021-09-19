#include <cassert>
#include <stdexcept>

#include "ast.h"
#include "string_utils.h"

WhileNode::WhileNode(IfWhileNodeCond *cond, std::vector<StatementNode *> stmt_lst, int stmt_no,
                     LocInfo loc)
    : StatementNode(stmt_no, loc) {
  this->cond_ = cond;
  this->stmt_lst_ = stmt_lst;
}

NodeType WhileNode::get_kind() { return NodeType::While; }

Node *WhileNode::get_cond() { return this->cond_; }

std::vector<StatementNode *> WhileNode::get_stmt_list() { return this->stmt_lst_; }

std::string WhileNode::ToString() {
  std::string res = StringFormat("WhileNode: {\nCond:\n%s\nStatements:", this->cond_->ToString());
  for (auto i = 0; i < this->stmt_lst_.size(); i++) {
    res += StringFormat("\n#%d:\n%s", i + 1, this->stmt_lst_[i]->ToString());
  }
  res += StringFormat("\nLine: %d\nCol: %d\n}", this->get_line_no(), this->get_col_no());

  return res;
}

#include <cassert>
#include <sstream>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"

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
  std::stringstream res;
  res << "WhileNode: {\n"
      << "Cond:\n"
      << this->cond_->ToString() + "\n"
      << "Statements:";

  for (auto i = 0; i < this->stmt_lst_.size(); i++) {
    res << "\n#" + std::to_string(i) + ":\n" << this->stmt_lst_[i]->ToString();
  }

  res << "\nLoc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

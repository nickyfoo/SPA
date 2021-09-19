#include <sstream>

#include "ast.h"
#include "ast_utils.hpp"

ProcedureNode::ProcedureNode(std::string name, std::vector<StatementNode *> stmt_lst, LocInfo loc)
    : Node(loc) {
  this->name_ = name;
  this->stmt_lst_ = stmt_lst;
}

NodeType ProcedureNode::get_kind() { return NodeType::Procedure; }

std::string ProcedureNode::get_name() { return this->name_; }

std::vector<StatementNode *> ProcedureNode::get_stmt_lst() { return this->stmt_lst_; }

std::string ProcedureNode::ToString() {
  std::stringstream res;
  res << "ProcedureNode: {\n"
      << "Name: " + this->name_ + "\n"
      << "Statements:";

  for (auto i = 0; i < this->stmt_lst_.size(); i++) {
    res << "\n#" + std::to_string(i) + ":\n" + this->stmt_lst_[i]->ToString();
  }
  res << "\nLoc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

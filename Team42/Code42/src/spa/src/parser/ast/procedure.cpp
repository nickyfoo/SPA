#include "ast.h"
#include "string_utils.h"

ProcedureNode::ProcedureNode(std::string name, std::vector<StatementNode *> stmt_lst, LocInfo loc)
    : Node(loc) {
  this->name_ = name;
  this->stmt_lst_ = stmt_lst;
}

NodeType ProcedureNode::get_kind() { return NodeType::Procedure; }

std::string ProcedureNode::get_name() { return this->name_; }

std::vector<StatementNode *> ProcedureNode::get_stmt_lst() { return this->stmt_lst_; }

std::string ProcedureNode::ToString() {
  std::string res = StringFormat("ProcedureNode:\nName: %s\nStatements:", this->name_); for (auto i = 0; i < this->stmt_lst_.size(); i++) {
    res += StringFormat("\n#%d:\n%s", i + 1, this->stmt_lst_[i]->ToString().c_str());
  }

  return res;
}

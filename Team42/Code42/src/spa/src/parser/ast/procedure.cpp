#include "ast.h"

ProcedureNode::ProcedureNode(std::string name,
                             std::vector<StatementNode *> stmt_lst, int line_no,
                             int col_no) {
  this->name_ = name;
  this->stmt_lst_ = stmt_lst;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType ProcedureNode::kind() { return NodeType::Procedure; }

int ProcedureNode::line_no() { return this->line_no_; }

int ProcedureNode::col_no() { return this->col_no_; }

std::string ProcedureNode::name() { return this->name_; }

std::vector<StatementNode *> ProcedureNode::stmt_lst() {
  return this->stmt_lst_;
}

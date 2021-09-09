#include "ast.h"

ProcedureNode::ProcedureNode(std::string name, std::vector<StatementNode *> stmt_lst, LocInfo loc)
    : Node(loc) {
  this->name_ = name;
  this->stmt_lst_ = stmt_lst;
}

NodeType ProcedureNode::kind() { return NodeType::Procedure; }

std::string ProcedureNode::name() { return this->name_; }

std::vector<StatementNode *> ProcedureNode::stmt_lst() { return this->stmt_lst_; }

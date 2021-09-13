#include "ast.h"

ProgramNode::ProgramNode(std::vector<ProcedureNode *> procedures, LocInfo loc) : Node(loc) {
  this->procedures_ = procedures;
}

NodeType ProgramNode::get_kind() { return NodeType::Program; }

std::vector<ProcedureNode *> ProgramNode::get_procedures() { return this->procedures_; }

#include "ast.h"

ProgramNode::ProgramNode(std::vector<ProcedureNode *> procedures, LocInfo loc) : Node(loc) {
  this->procedures_ = procedures;
}

NodeType ProgramNode::kind() { return NodeType::Program; }

std::vector<ProcedureNode *> ProgramNode::procedures() { return this->procedures_; }

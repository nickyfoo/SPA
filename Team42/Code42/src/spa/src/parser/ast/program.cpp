#include "ast.h"
#include "string_utils.h"

ProgramNode::ProgramNode(std::vector<ProcedureNode *> procedures, LocInfo loc) : Node(loc) {
  this->procedures_ = procedures;
}

NodeType ProgramNode::get_kind() { return NodeType::Program; }

std::vector<ProcedureNode *> ProgramNode::get_procedures() { return this->procedures_; }

std::string ProgramNode::ToString() {
  std::string res = "ProgramNode:\nProcedures:";
  for (auto i = 0; i < this->procedures_.size(); i++) {
    res += StringFormat("\n#%d:\n%s", i + 1, this->procedures_[i]->ToString().c_str());
  }

  return res;
}

#include <sstream>

#include "ast.h"
#include "ast_utils.hpp"

ProgramNode::ProgramNode(std::vector<ProcedureNode *> procedures, LocInfo loc) : Node(loc) {
  this->procedures_ = procedures;
}

NodeType ProgramNode::get_kind() { return NodeType::Program; }

std::vector<ProcedureNode *> ProgramNode::get_procedures() { return this->procedures_; }

std::string ProgramNode::ToString() {
  std::stringstream res;
  res << "ProgramNode: {\n"
      << "Procedures:\n";

  for (auto i = 0; i < this->procedures_.size(); i++) {
    res << "\n#" + std::to_string(i) + ":\n" << this->procedures_[i]->ToString();
  }

  res << "\nLoc: " + LocToString(this->get_line_no(), this->get_col_no()) + "\n"
      << "}\n";

  return res.str();
}

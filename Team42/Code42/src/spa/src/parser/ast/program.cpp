#include "ast.h"

ProgramNode::ProgramNode(std::vector<ProcedureNode *> procedures, int line_no,
                         int col_no) {
  this->procedures_ = procedures;
  this->line_no_ = line_no;
  this->col_no_ = col_no;
}

NodeType ProgramNode::kind() { return NodeType::Program; }

int ProgramNode::line_no() { return this->line_no_; }

int ProgramNode::col_no() { return this->col_no_; }

std::vector<ProcedureNode *> ProgramNode::procedures() {
  return this->procedures_;
}

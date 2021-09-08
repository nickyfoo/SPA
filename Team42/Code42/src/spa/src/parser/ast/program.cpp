#include "ast.h"

ProgramNode::ProgramNode(std::vector<ProcedureNode *> procedures, int line_no,
                         int col_no) {}
NodeType ProgramNode::kind() {}
int ProgramNode::line_no() {}
int ProgramNode::col_no() {}
std::vector<ProcedureNode *> ProgramNode::procedures() {}

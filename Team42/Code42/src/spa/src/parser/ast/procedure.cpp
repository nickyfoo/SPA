#include "ast.h"

ProcedureNode::ProcedureNode(std::string name,
                             std::vector<StatementNode *> stmt_lst, int line_no,
                             int col_no) {}
NodeType ProcedureNode::kind() {}
int ProcedureNode::line_no() {}
int ProcedureNode::col_no() {}
std::string ProcedureNode::name() {}
std::vector<StatementNode *> ProcedureNode::stmt_lst() {}

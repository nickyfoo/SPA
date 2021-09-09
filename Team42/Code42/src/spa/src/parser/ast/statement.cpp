#include <stdexcept>

#include "ast.h"

StatementNode::StatementNode(int stmt_no, LocInfo loc) : Node(loc) { this->stmt_no_ = stmt_no; }

int StatementNode::stmt_no() { return this->stmt_no_; }

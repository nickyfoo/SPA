#include "ast.h"

CallNode::CallNode(IdentifierNode *proc, int stmt_no, int line_no, int col_no) {
}
NodeType CallNode::kind() {}
int CallNode::line_no() {}
int CallNode::col_no() {}
int CallNode::stmt_no() {}
IdentifierNode CallNode::*proc() {}

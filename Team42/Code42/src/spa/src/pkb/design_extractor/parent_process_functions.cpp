#include "pkb.h"
#include "ast_utils.hpp"

void PKB::ParentProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode *>(node);
  Statement *if_statement = stmt_table_.get_statement(if_node->get_stmt_no());
  for (StatementNode *n : if_node->get_then_stmt_lst()) {
    if_statement->AddChild(n->get_stmt_no());
    stmt_table_.get_statement(n->get_stmt_no())->AddParent(if_statement->get_stmt_no());
  }

  for (StatementNode *n : if_node->get_else_stmt_lst()) {
    if_statement->AddChild(n->get_stmt_no());
    stmt_table_.get_statement(n->get_stmt_no())->AddParent(if_statement->get_stmt_no());
  }
}

void PKB::ParentProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  Statement *while_statement = stmt_table_.get_statement(while_node->get_stmt_no());
  for (StatementNode *n : while_node->get_stmt_list()) {
    while_statement->AddChild(n->get_stmt_no());
    stmt_table_.get_statement(n->get_stmt_no())->AddParent(while_statement->get_stmt_no());
  }
}

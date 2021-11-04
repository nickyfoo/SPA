#include <design_extractor.h>
#include "ast_utils.hpp"

void DesignExtractor::ParentProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode *>(node);
  int if_stmt_no = if_node->get_stmt_no();
  Statement *if_stmt = pkb_->stmt_table_.get_statement(if_stmt_no);

  // Add statements in then/else stmt lists as child stmts and vice versa
  for (StatementNode *n : if_node->get_then_stmt_lst()) {
    int stmt_no = n->get_stmt_no();

    if_stmt->AddChild(stmt_no);
    pkb_->stmt_table_.get_statement(stmt_no)->AddParent(if_stmt_no);
  }
  for (StatementNode *n : if_node->get_else_stmt_lst()) {
    int stmt_no = n->get_stmt_no();

    if_stmt->AddChild(stmt_no);
    pkb_->stmt_table_.get_statement(stmt_no)->AddParent(if_stmt_no);
  }
}

void DesignExtractor::ParentProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  Statement *while_stmt = pkb_->stmt_table_.get_statement(while_node->get_stmt_no());

  // Add statements in stmt list as child stmts and vice versa
  for (StatementNode *n : while_node->get_stmt_list()) {
    while_stmt->AddChild(n->get_stmt_no());
    pkb_->stmt_table_.get_statement(n->get_stmt_no())->AddParent(while_stmt->get_stmt_no());
  }
}

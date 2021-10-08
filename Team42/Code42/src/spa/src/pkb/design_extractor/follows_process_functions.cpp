#include "pkb.h"
#include <algorithm>
#include <vector>
#include "ast_utils.hpp"
#include "pkb_exception.h"

void PKB::FollowsProcessProcedureNode(Node *node) {
  auto *procedure_node = dynamic_cast<ProcedureNode *>(node);
  std::vector<int> line_no;

  for (StatementNode *n : procedure_node->get_stmt_lst()) {
    line_no.push_back(n->get_stmt_no());
  }
  sort(line_no.begin(), line_no.end());
  for (int i = 1; i < line_no.size(); i++) {
    stmt_table_.get_statement(line_no[i - 1])->AddFollower(line_no[i]);
    stmt_table_.get_statement(line_no[i])->AddFollowee(line_no[i - 1]);
  }
}

void PKB::FollowsProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode *>(node);
  std::vector<int> then_line_nos, else_line_nos;

  for (StatementNode *n : if_node->get_then_stmt_lst()) {
    then_line_nos.push_back(n->get_stmt_no());
  }
  std::sort(then_line_nos.begin(), then_line_nos.end());
  for (int i = 1; i < then_line_nos.size(); i++) {
    stmt_table_.get_statement(then_line_nos[i - 1])->AddFollower(then_line_nos[i]);
    stmt_table_.get_statement(then_line_nos[i])->AddFollowee(then_line_nos[i - 1]);
  }

  for (StatementNode *n : if_node->get_else_stmt_lst()) {
    else_line_nos.push_back(n->get_stmt_no());
  }
  std::sort(else_line_nos.begin(), else_line_nos.end());
  for (int i = 1; i < else_line_nos.size(); i++) {
    stmt_table_.get_statement(else_line_nos[i - 1])->AddFollower(else_line_nos[i]);
    stmt_table_.get_statement(else_line_nos[i])->AddFollowee(else_line_nos[i - 1]);
  }
}

void PKB::FollowsProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  std::vector<int> line_nos;
  for (StatementNode *n : while_node->get_stmt_list()) {
    line_nos.push_back(n->get_stmt_no());
  }
  sort(line_nos.begin(), line_nos.end());
  for (int i = 1; i < line_nos.size(); i++) {
    stmt_table_.get_statement(line_nos[i - 1])->AddFollower(line_nos[i]);
    stmt_table_.get_statement(line_nos[i])->AddFollowee(line_nos[i - 1]);
  }
}

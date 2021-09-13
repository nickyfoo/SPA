#include "stmt_table.h"
#include <iostream>

StmtTable::StmtTable() {
  num_statements_ = 0;
}

StmtTable::~StmtTable() = default;

void StmtTable::AddStatement(Node *node) {
  StatementNode *stmt_node = dynamic_cast<StatementNode *>(node);
  int stmt_no = stmt_node->get_stmt_no();
  // Return if statement number is invalid
  if (stmt_no == 0) return;
  Statement s(stmt_no, stmt_node->get_kind());
  table_[stmt_no] = s;
  type_to_statement_[stmt_node->get_kind()].push_back(&table_[stmt_no]);
  all_statements_.push_back(&table_[stmt_no]);
  num_statements_ = std::max(num_statements_, stmt_no);
}

int StmtTable::get_num_statements() {
  return num_statements_;
}

std::vector<Statement *> StmtTable::get_all_statements() {
  return all_statements_;
}

std::vector<Statement *> StmtTable::get_statements(NodeType type) {
  return type_to_statement_[type];
}

Statement *StmtTable::get_statement(int line_no) {
  auto it = table_.find(line_no);
  if (it == table_.end()) return nullptr;
  return &(it->second);
}

void StmtTable::ProcessFollows() {
  for (auto&[line_no, stmt] : table_) {
    for (auto &followerLineNo : *(stmt.get_followers())) {
      follows_.insert({line_no, followerLineNo});
    }
  }
}

void StmtTable::ProcessFollowsStar() {
  int n = num_statements_ + 1;
  std::vector<std::vector<int>> d = GetTransitiveClosure(follows_, n);
  for (int i = 0; i < n; i++) {
    Statement *stmt = get_statement(i);
    if (stmt == nullptr) continue;

    for (int j = 0; j < n; j++) {
      if (d[i][j] == kInf) continue;
      stmt->AddFollowerStar(j);
      get_statement(j)->AddFolloweeStar(i);
    }
  }
}

// Gets Follow relationship from Statements in preparation to get transitive closure.
void StmtTable::ProcessParent() {
  for (auto&[line_no, stmt] : table_) {
    for (auto &child_line_no : *(stmt.get_children())) {
      parent_.insert({line_no, child_line_no});
    }
  }
}

void StmtTable::ProcessParentStar() {
  int n = num_statements_ + 1;
  std::vector<std::vector<int>> d = GetTransitiveClosure(parent_, n);
  for (int i = 0; i < n; i++) {
    Statement *stmt = get_statement(i);
    if (stmt == nullptr) continue;

    for (int j = 0; j < n; j++) {
      if (d[i][j] == kInf) continue;
      stmt->AddChildStar(j);
      get_statement(j)->AddParentStar(i);
    }
  }
}

void StmtTable::PrintStatements() {
  std::cout << "StmtTable size: " << table_.size() << '\n';
  for (auto&[k, x] : table_) {
    std::cout << k << ": " << static_cast<int>(x.get_kind()) << ' ';
    if (x.get_kind() == NodeType::Assign) std::cout << x.get_expr_string();
    std::cout << '\n';
  }
}

void StmtTable::PrintStatementDetails() {
  for (auto&[k, x] : table_) {
    x.FollowsInfo();
    x.ParentInfo();
  }
}

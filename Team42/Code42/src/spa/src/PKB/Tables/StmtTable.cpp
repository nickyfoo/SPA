#include "StmtTable.h"
#include <iostream>

StmtTable::StmtTable() {
  num_statements_ = 0;
}

StmtTable::~StmtTable() = default;

void StmtTable::AddStatement(ast::Node *node) {
  int stmt_no = Statement::GetStmtNo(node);
  // Return if statement number is invalid
  if (stmt_no == 0) return;
  Statement s(stmt_no, node->kind);
  table_[stmt_no] = s;
  type_to_statement_[node->kind].push_back(&table_[stmt_no]);
  all_statements_.push_back(&table_[stmt_no]);
  num_statements_ = std::max(num_statements_, stmt_no);
}

int StmtTable::GetNumStatements() {
  return num_statements_;
}

std::vector<Statement *> StmtTable::GetStatements(ast::Kind type) {
  return type_to_statement_[type];
}

Statement *StmtTable::GetStatement(int line_no) {
  auto it = table_.find(line_no);
  if (it == table_.end()) return nullptr;
  return &(it->second);
}

std::vector<Statement *> StmtTable::GetAllStatements() {
  return all_statements_;
}

void StmtTable::ProcessFollows() {
  for (auto&[line_no, stmt] : table_) {
    for (auto &followerLineNo : *(stmt.GetFollowers())) {
      follows_.insert({line_no, followerLineNo});
    }
  }
}

void StmtTable::ProcessFollowsStar() {
  int n = num_statements_ + 1;
  std::vector<std::vector<int>> d = GetTransitiveClosure(follows_, n);
  for (int i = 0; i < n; i++) {
    Statement *stmt = GetStatement(i);
    if (stmt == nullptr) continue;

    for (int j = 0; j < n; j++) {
      if (d[i][j] == kInf) continue;
      stmt->AddFollowerStar(j);
      GetStatement(j)->AddFolloweeStar(i);
    }
  }
}

// Gets Follow relationship from Statements in preparation to get transitive closure.
void StmtTable::ProcessParent() {
  for (auto&[line_no, stmt] : table_) {
    for (auto &child_line_no : *(stmt.GetChildren())) {
      parent_.insert({line_no, child_line_no});
    }
  }
}

void StmtTable::ProcessParentStar() {
  int n = num_statements_ + 1;
  std::vector<std::vector<int>> d = GetTransitiveClosure(parent_, n);
  for (int i = 0; i < n; i++) {
    Statement *stmt = GetStatement(i);
    if (stmt == nullptr) continue;

    for (int j = 0; j < n; j++) {
      if (d[i][j] == kInf) continue;
      stmt->AddChildStar(j);
      GetStatement(j)->AddParentStar(i);
    }
  }
}

void StmtTable::PrintStatements() {
  std::cout << "StmtTable size: " << table_.size() << '\n';
  for (auto&[k, x] : table_) {
    std::cout << k << ": " << x.GetKind() << ' ';
    if (x.GetKind() == ast::Assign) std::cout << x.GetExprString();
    std::cout << '\n';
  }
}

void StmtTable::PrintStatementDetails() {
  for (auto&[k, x] : table_) {
    x.FollowsInfo();
    x.ParentInfo();
  }
}

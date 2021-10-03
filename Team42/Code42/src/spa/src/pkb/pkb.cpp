#include "pkb.h"
#include <algorithm>
#include <vector>
#include <queue>
#include "ast_utils.hpp"

PKB::PKB() = default;

PKB::PKB(Node *programRoot) {
  this->root_ = programRoot;
  Initialise();
}

PKB::~PKB() = default;

void PKB::AddProcedure(Node *node, std::vector<Node *> ancestor_list) {
  auto *procedure_node = dynamic_cast<ProcedureNode *>(node);
  proc_table_.AddProcedure(procedure_node->get_name());
}

void PKB::AddStatement(Node *node, std::vector<Node *> ancestor_list) {
  stmt_table_.AddStatement(node);
}

void PKB::AddExprString(Node *node, std::vector<Node *> ancestor_list) {
  if (node->get_kind() == NodeType::Assign) {
    auto *assign_node = dynamic_cast<AssignNode *>(node);
    switch (assign_node->expr()->get_kind()) {
      case NodeType::Expression: {
        auto *expression_node = dynamic_cast<ExpressionNode *>(assign_node->expr());
        std::string expr_string = expression_node->get_expr_string();
        stmt_table_.get_statement(assign_node->get_stmt_no())->set_expr_string(expr_string);
        break;
      }
      case NodeType::Constant: {
        auto *constant_node = dynamic_cast<ConstantNode *>(assign_node->expr());
        std::string expr_string = constant_node->get_expr_string();
        stmt_table_.get_statement(assign_node->get_stmt_no())->set_expr_string(expr_string);
        break;
      }
      case NodeType::Identifier: {
        auto *identifier_node = dynamic_cast<IdentifierNode *>(assign_node->expr());
        std::string expr_string = identifier_node->get_expr_string();
        stmt_table_.get_statement(assign_node->get_stmt_no())->set_expr_string(expr_string);
        break;
      }
      default:
        // TODO(nic): might throw an error here
        break;
    }
  }

  if (node->get_kind() == NodeType::If) {
    auto *if_node = dynamic_cast<IfNode *>(node);
    switch (if_node->get_cond()->get_kind()) {
      case NodeType::RelExpression: {
        auto *rel_expression_node =
            dynamic_cast<RelExpressionNode *>(if_node->get_cond());
        std::string expr_string = rel_expression_node->get_expr_string();
        stmt_table_.get_statement(if_node->get_stmt_no())
            ->set_expr_string(expr_string);
        break;
      }
      case NodeType::CondExpression: {
        auto *cond_expression_node = dynamic_cast<CondExpressionNode *>(if_node->get_cond());
        std::string expr_string = cond_expression_node->get_expr_string();
        stmt_table_.get_statement(if_node->get_stmt_no())
            ->set_expr_string(expr_string);
        break;
      }
      default:break;
    }
  }

  if (node->get_kind() == NodeType::While) {
    auto *while_node = dynamic_cast<WhileNode *>(node);
    switch (while_node->get_cond()->get_kind()) {
      case NodeType::RelExpression: {
        auto *rel_expression_node =
            dynamic_cast<RelExpressionNode *>(while_node->get_cond());
        std::string expr_string = rel_expression_node->get_expr_string();
        stmt_table_.get_statement(while_node->get_stmt_no())
            ->set_expr_string(expr_string);
        break;
      }
      case NodeType::CondExpression: {
        auto *cond_expression_node =
            dynamic_cast<CondExpressionNode *>(while_node->get_cond());
        std::string expr_string = cond_expression_node->get_expr_string();
        stmt_table_.get_statement(while_node->get_stmt_no())
            ->set_expr_string(expr_string);
        break;
      }
      default:break;
    }
  }

}

void PKB::AddVariable(Node *node, std::vector<Node *> ancestor_list) {
  auto *identifier_node = dynamic_cast<IdentifierNode *>(node);
  if (ancestor_list.back()->get_kind() != NodeType::Call) {
    var_table_.AddVariable(identifier_node->get_name());
  }
}

void PKB::AddConstant(Node *node, std::vector<Node *> ancestor_list) {
  auto *constant_node = dynamic_cast<ConstantNode *>(node);
  const_table_.AddConstant(constant_node->get_value());
}

int PKB::get_num_procedures() {
  return proc_table_.get_num_procedures();
}

std::vector<Procedure *> PKB::get_all_procedures() {
  return proc_table_.get_all_procedures();
}

Procedure *PKB::get_procedure(std::string &name) {
  return proc_table_.get_procedure(name);
}

int PKB::get_num_statements() {
  return stmt_table_.get_num_statements();
}

std::vector<Statement *> PKB::get_all_statements() {
  return stmt_table_.get_all_statements();
}

std::vector<Statement *> PKB::get_statements(NodeType type) {
  return stmt_table_.get_statements(type);
}

Statement *PKB::get_statement(int line_no) {
  return stmt_table_.get_statement(line_no);
}

std::vector<Variable *> PKB::get_all_variables() {
  return var_table_.get_all_variables();
}

std::vector<Constant *> PKB::get_all_constants() {
  return const_table_.get_all_constants();
}

std::map<int, std::set<int>> *PKB::get_cfgal() {
  return &CFGAL_;
}

std::map<int, std::set<int>> *PKB::get_reverse_cfgal() {
  return &ReverseCFGAL_;
}

std::set<std::pair<int, int>> PKB::get_next(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  if (a == kWild && b == kWild) {
    for (auto&[u, al_u] : CFGAL_) {
      for (auto &v : al_u) {
        ans.insert({u, v});
      }
    }
  } else if (a == kWild && b != kWild) {
    for (auto &v : ReverseCFGAL_[b]) {
      ans.insert({v, b});
    }
  } else if (a != kWild && b == kWild) {
    for (auto &v : CFGAL_[a]) {
      ans.insert({a, v});
    }
  } else {
    if (CFGAL_[a].find(b) != CFGAL_[a].end()) {
      ans.insert({a, b});
    }
  }
  return ans;
}

std::set<std::pair<int, int>> PKB::get_next_star(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    for (int i = 0; i < n; i++) {
      ReachabilityDFS(i, i, d, CFGAL_);
    }
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) ans.insert({i, j});
      }
    }
  } else if (a == kWild && b != kWild) {
    ReachabilityDFS(b, b, d, ReverseCFGAL_);
    for (int i = 0; i < n; i++) {
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[b][i] != 0) ans.insert({i, b});
    }
  } else if (a != kWild && b == kWild) {
    ReachabilityDFS(a, a, d, CFGAL_);
    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) ans.insert({a, j});
    }
  } else {
    ReachabilityDFS(a, a, d, CFGAL_);
    if (d[a][b] != 0) ans.insert({a, b});
  }
  return ans;
}

std::set<std::pair<int, int>> PKB::get_affects(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    for (auto &stmt : stmt_table_.get_statements(NodeType::Assign)) {
      if (stmt->get_modifies()->size() != 1) continue;
      std::vector<bool> visited(n, false);
      std::string var_name = *(stmt->get_modifies()->begin());
      bool found = false;
      AffectsDFS(stmt->get_stmt_no(), kWild, stmt->get_stmt_no(), var_name, visited, d, found);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) ans.insert({i, j});
      }
    }
  } else if (a == kWild && b != kWild) {
    Statement *stmt = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;

    std::vector<bool> stmt_checked(n, false);
    for (auto &var_used : *(stmt->get_uses())) {
      for (auto &stmt_using : *(var_table_.get_variable(var_used)->get_stmts_modifying())) {
        if (!stmt_checked[stmt_using]) {
          stmt_checked[stmt_using] = true;
          if (stmt_table_.get_statement(stmt_using)->get_kind() != NodeType::Assign) continue;
          std::vector<bool> visited(n, false);
          bool found = false;
          AffectsDFS(stmt_using, b, stmt_using, var_used, visited, d, found);
        }
      }
    }

    for (int i = 0; i < n; i++) {
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[i][b] != 0) ans.insert({i, b});
    }
  } else if (a != kWild && b == kWild) {
    Statement *stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    if (stmt->get_modifies()->size() != 1) return ans;
    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt->get_modifies()->begin());
    AffectsDFS(stmt->get_stmt_no(), kWild, stmt->get_stmt_no(), var_name, visited, d, found);

    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) ans.insert({a, j});
    }
  } else {
    Statement *stmt = stmt_table_.get_statement(a);
    Statement *stmt2 = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    if (stmt2 == nullptr || stmt2->get_kind() != NodeType::Assign) return ans;
    if (stmt->get_modifies()->size() != 1) return ans;
    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt->get_modifies()->begin());
    AffectsDFS(stmt->get_stmt_no(), b, stmt->get_stmt_no(), var_name, visited, d, found);

    if (d[a][b] != 0) ans.insert({a, b});
  }
  return ans;
}

std::set<std::pair<int, int>> PKB::get_affects_star(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == kWild && b == kWild) {
    std::map<int, std::set<int>> affects_al;
    for (auto&[a, b] : get_affects(kWild, kWild)) {
      affects_al[a].insert(b);
    }
    for (auto&[u, al_u] : affects_al) {
      ReachabilityDFS(u, u, d, affects_al);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) ans.insert({i, j});
      }
    }
  } else if (a == kWild && b != kWild) {
    Statement *stmt = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    std::vector<bool> visited(n, false);
    AffectsStarBFS(b, kWild, visited, ans, false);
  } else if (a != kWild && b == kWild) {
    Statement *stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, 0, visited, ans, true);
  } else {
    Statement *stmt = stmt_table_.get_statement(a);
    Statement *stmt2 = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    if (stmt2 == nullptr || stmt2->get_kind() != NodeType::Assign) return ans;
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, b, visited, ans, true);
  }
  return ans;
}

bool PKB::TestAssignmentPattern(Statement *statement, std::string pattern, bool is_partial_match) {
  return PatternManager::TestAssignmentPattern(statement, pattern, is_partial_match);
}

void PKB::PrintStatements() {
  stmt_table_.PrintStatements();
}

void PKB::PrintProcedures() {
  proc_table_.PrintProcedureDetails();
}

void PKB::PrintVariables() {
  var_table_.PrintVariableDetails();
}

void PKB::PrintCFGAL() {
  for (auto &[u, al] : CFGAL_) {
    std::cout << u << "->";
    for (auto &v : al) {
      std::cout << v << ' ';
    }
    std::cout << '\n';
  }
}

void PKB::Initialise() {
  ExtractEntities();
  ExtractFollows();
  ExtractParent();
  ExtractCalls();
  ExtractUsesModifies();
  ExtractCFG();
}

#include "pkb.h"
#include <algorithm>
#include <vector>
#include <queue>
#include "ast_utils.hpp"
#include "pkb_exception.h"

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

std::map<int, std::set<int>>* PKB::get_reverse_cfgal() {
  return &ReverseCFGAL_;
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

void PKB::ExtractEntities() {
  auto extract_variable = [this](Node *node,
                                 std::vector<Node *> ancestor_list) {
    PKB::AddVariable(node, ancestor_list);
  };
  auto extract_constant = [this](Node *node,
                                 std::vector<Node *> ancestor_list) {
    PKB::AddConstant(node, ancestor_list);
  };
  auto extract_statement = [this](Node *node,
                                  std::vector<Node *> ancestor_list) {
    PKB::AddStatement(node, ancestor_list);
  };
  auto extract_expr_string = [this](Node *node,
                                    std::vector<Node *> ancestor_list) {
    PKB::AddExprString(node, ancestor_list);
  };
  auto extract_procedure = [this](Node *node,
                                  std::vector<Node *> ancestor_list) {
    PKB::AddProcedure(node, ancestor_list);
  };

  std::map<NodeType, std::vector<std::function<void(Node *, std::vector<Node *>)>>> functions = {
      {NodeType::Identifier, {extract_variable}},
      {NodeType::Constant, {extract_constant}},
      {NodeType::Assign, {extract_statement, extract_expr_string}},
      {NodeType::If, {extract_statement, extract_expr_string}},
      {NodeType::While, {extract_statement, extract_expr_string}},
      {NodeType::Read, {extract_statement}},
      {NodeType::Print, {extract_statement}},
      {NodeType::Call, {extract_statement}},
      {NodeType::Procedure, {extract_procedure}},
  };

  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);
  stmt_table_.CategorizeStatements();
}

void PKB::ExtractFollows() {
  auto extract_follows_for_if_node = [this](Node *node) {
    PKB::FollowsProcessIfNode(node);
  };
  auto extract_follows_for_while_node = [this](Node *node) {
    PKB::FollowsProcessWhileNode(node);
  };
  auto extract_follows_for_procedure_node = [this](Node *node) {
    PKB::FollowsProcessProcedureNode(node);
  };

  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_follows_for_if_node}},
      {NodeType::While, {extract_follows_for_while_node}},
      {NodeType::Procedure, {extract_follows_for_procedure_node}},
  };
  Visit(root_, functions);
  stmt_table_.ProcessFollows();
  stmt_table_.ProcessFollowsStar();
}

void PKB::ExtractParent() {
  auto extract_parents_for_if_node = [this](Node *node) { PKB::ParentProcessIfNode(node); };
  auto extract_parents_for_while_node = [this](Node *node) { PKB::ParentProcessWhileNode(node); };

  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_parents_for_if_node}},
      {NodeType::While, {extract_parents_for_while_node}},
  };
  Visit(root_, functions);
  stmt_table_.ProcessParent();
  stmt_table_.ProcessParentStar();
}

void PKB::ExtractUsesModifies() {
  auto extract_uses_modifies_for_assign_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::UsesModifiesProcessAssignNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_if_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::UsesModifiesProcessIfNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_while_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::UsesModifiesProcessWhileNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_read_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::UsesModifiesProcessReadNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_print_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::UsesModifiesProcessPrintNode(node, ancestor_list);
      };

  std::map<NodeType, std::vector<std::function<void(Node *, std::vector<Node *>)>>> functions = {
      {NodeType::Assign, {extract_uses_modifies_for_assign_node}},
      {NodeType::If, {extract_uses_modifies_for_if_node}},
      {NodeType::While, {extract_uses_modifies_for_while_node}},
      {NodeType::Read, {extract_uses_modifies_for_read_node}},
      {NodeType::Print, {extract_uses_modifies_for_print_node}},
  };
  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);
  proc_table_.ProcessUsesModifiesIndirect();
  UpdateVarTableWithProcs();
  for (auto &call_statement : get_statements(NodeType::Call)) {
    std::string called_procedure = call_statement->get_called_proc_name();
    auto uses_variables = proc_table_.get_procedure(called_procedure)->get_uses();
    auto modifies_variables = proc_table_.get_procedure(called_procedure)->get_modifies();
    for (auto &var_used : *uses_variables) {
      call_statement->AddUses(var_used);
      var_table_.get_variable(var_used)->AddStmtUsing(call_statement->get_stmt_no());
    }
    for (auto &var_modified : *modifies_variables) {
      call_statement->AddModifies(var_modified);
      var_table_.get_variable(var_modified)->AddStmtModifying(
          call_statement->get_stmt_no());
    }

    // Propagating variables in call statements to container statements.
    for (auto &parent_star : *(call_statement->get_parents_star())) {
      for (auto &var_used : *uses_variables) {
        stmt_table_.get_statement(parent_star)->AddUses(var_used);
        var_table_.get_variable(var_used)->AddStmtUsing(parent_star);
      }
      for (auto &var_modified : *modifies_variables) {
        stmt_table_.get_statement(parent_star)->AddModifies(var_modified);
        var_table_.get_variable(var_modified)->AddStmtModifying(parent_star);
      }
    }

  }
}

void PKB::ExtractCalls() {
  auto extract_calls_for_call_node = [this](Node *node, std::vector<Node *> ancestor_list) {
    PKB::CallsProcessCallNode(node, ancestor_list);
  };

  std::map<
      NodeType,
      std::vector<std::function<void(Node *, std::vector<Node *>)>>>
      functions = {
      {NodeType::Call, {extract_calls_for_call_node}},
  };
  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);

  proc_table_.ProcessCalls();
  proc_table_.ProcessCallsStar();
}

void PKB::ExtractCFG() {
  auto extract_cfg_for_if_node = [this](Node *node) {
    PKB::CFGProcessIfNode(node);
  };
  auto extract_cfg_for_while_node = [this](Node *node) {
    PKB::CFGProcessWhileNode(node);
  };
  auto extract_cfg_for_procedure_node = [this](Node *node) {
    PKB::CFGProcessProcedureNode(node);
  };


  std::map<NodeType, std::vector<std::function<void(Node*)>>> functions = {
      {NodeType::If, {extract_cfg_for_if_node}},
      {NodeType::While, {extract_cfg_for_while_node}},
      {NodeType::Procedure, {extract_cfg_for_procedure_node}},
  };

  Visit(root_, functions);

}

void PKB::ReachabilityDFS(int start, int u, std::vector<std::vector<int>>&d, std::map<int, std::set<int>>& al) {
  for (auto& v : al[u]) {
    if (d[start][v] == 0) {
      d[start][v] = 1;
      ReachabilityDFS(start, v, d, al);
    }
  }
}

std::set<std::pair<int, int>> PKB::get_next(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  if (a == 0 && b == 0) {
    for (auto& [u, al_u] : CFGAL_) {
      for (auto& v : al_u) {
        ans.insert({ u,v });
      }
    }
  }
  else if (a == 0 && b != 0) {
    for (auto& v : ReverseCFGAL_[b]) {
      ans.insert({ v,b });
    }
  }
  else if (a != 0 && b == 0) {
    for (auto& v : CFGAL_[a]) {
      ans.insert({ a,v });
    }
  }
  else {
    if (CFGAL_[a].find(b) != CFGAL_[a].end()) {
      ans.insert({ a,b });
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
  if (a == 0 && b == 0) {
    for (int i = 0; i < n; i++) {
      ReachabilityDFS(i, i, d, CFGAL_);
    }
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) ans.insert({ i,j });
      }
    }
  }
  else if (a == 0 && b != 0) {
    ReachabilityDFS(b, b, d, ReverseCFGAL_);
    for (int i = 0; i < n; i++) {
      // Be careful about the check, d[i][j] means that i can reach j!
      if (d[b][i] != 0) ans.insert({ i,b });
    }
  }
  else if (a != 0 && b == 0) {
    ReachabilityDFS(a, a, d, CFGAL_);
    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) ans.insert({ a,j });
    }
  }
  else {
    ReachabilityDFS(a, a, d, CFGAL_);
    if (d[a][b] != 0) ans.insert({ a,b });
  }
  return ans;
}


void PKB::AffectsDFS(int start, int target, int u, std::string var_name, std::vector<bool>&visited, std::vector<std::vector<int>>&d, bool&found) {
  if (found) return;
  Statement *stmt = stmt_table_.get_statement(start);
  for (auto &v : CFGAL_[u]) {
    Statement *stmt_v = stmt_table_.get_statement(v);
    if (!visited[v]) {
      visited[v] = true;
      std::set<std::string> *uses = stmt_v->get_uses();
      if (stmt_v->get_kind() == NodeType::Assign && uses->find(var_name) != uses->end()) {
        d[start][v] = 1;
        if (v==target){
          found = true;
          return;
        }
      }
      std::set<std::string> *modifies = stmt_v->get_modifies();
      if (stmt_v->get_kind() == NodeType::Assign || stmt_v->get_kind() == NodeType::Read || stmt_v->get_kind() == NodeType::Call) {
        if (modifies->find(var_name) != modifies->end()) {
          continue;
        }
      }
      AffectsDFS(start, target, v, var_name, visited, d, found);
    }
  }
}

void PKB::AffectsStarBFS(int start, int target, std::vector<bool>& visited, std::set<std::pair<int, int>>& ans, bool forward_relation) {
  std::queue<int> q;
  visited[start] = true;
  q.push(start);
  while (q.size()) {
    int u = q.front(); q.pop();
    if (forward_relation) {
      for (auto& [a, b] : get_affects(u, 0)) {
        if(target==0) ans.insert({ start, b });
        else {
          if (b == target) {
            ans.insert({ start,target });
            return;
          }
        }
        if (!visited[b]) {
          visited[b] = true;
          q.push(b);
        }
      }
    }
    else {
      for (auto& [a, b] : get_affects(0, u)) {
        if (target == 0) ans.insert({ a,start });
        else {
          if (a == target) {
            ans.insert({ start,target });
            return;
          }
        }
        if (!visited[a]) {
          visited[a] = true;
          q.push(a);
        }
      }
    }
  }
}

std::set<std::pair<int, int>> PKB::get_affects(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == 0 && b == 0) {
    for (auto& stmt : stmt_table_.get_statements(NodeType::Assign)) {
      if (stmt->get_modifies()->size() != 1) continue;
      std::vector<bool> visited(n, false);
      std::string var_name = *(stmt->get_modifies()->begin());
      bool found = false;
      AffectsDFS(stmt->get_stmt_no(), 0, stmt->get_stmt_no(), var_name, visited, d, found);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) ans.insert({ i,j });
      }
    }
  }
  else if (a == 0 && b != 0) {
    Statement* stmt = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind()!=NodeType::Assign) return ans;
    
    std::vector<bool> stmt_checked(n, false);
    for (auto& var_used : *(stmt->get_uses())) {
      for (auto& stmt_using : *(var_table_.get_variable(var_used)->get_stmts_modifying())) {
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
      if (d[i][b] != 0) ans.insert({ i,b });
    }
  }
  else if (a != 0 && b == 0) {
    Statement* stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    if (stmt->get_modifies()->size() != 1) return ans;
    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt->get_modifies()->begin());
    AffectsDFS(stmt->get_stmt_no(), 0, stmt->get_stmt_no(), var_name, visited, d, found);

    for (int j = 0; j < n; j++) {
      if (d[a][j] != 0) ans.insert({ a,j });
    }
  }
  else {
    Statement* stmt = stmt_table_.get_statement(a);
    Statement* stmt2 = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    if (stmt2 == nullptr || stmt2->get_kind() != NodeType::Assign) return ans;
    if (stmt->get_modifies()->size() != 1) return ans;
    std::vector<bool> visited(n, false);
    bool found = false;
    std::string var_name = *(stmt->get_modifies()->begin());
    AffectsDFS(stmt->get_stmt_no(), b, stmt->get_stmt_no(), var_name, visited, d, found);

    if (d[a][b] != 0) ans.insert({ a,b });
  }
  return ans;
}

std::set<std::pair<int, int>> PKB::get_affects_star(int a, int b) {
  int n = stmt_table_.get_num_statements() + 1;
  std::set<std::pair<int, int>> ans;
  // Invalid line nums
  if (a < 0 || a >= n || b < 0 || b >= n) return ans;
  std::vector<std::vector<int>> d(n, std::vector<int>(n, 0));
  if (a == 0 && b == 0) {
    std::map<int, std::set<int>> affects_al;
    for (auto& [a, b] : get_affects(0, 0)) {
      affects_al[a].insert(b);
    }
    for (auto&[u,al_u] : affects_al) {
      ReachabilityDFS(u, u, d, affects_al);
    }

    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
        if (d[i][j] != 0) ans.insert({ i,j });
      }
    }
  }
  else if (a == 0 && b != 0) {
    Statement* stmt = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    std::vector<bool> visited(n, false);
    AffectsStarBFS(b, 0, visited, ans, false);
  }
  else if (a != 0 && b == 0) {
    Statement* stmt = stmt_table_.get_statement(a);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, 0, visited, ans, true);
  }
  else {
    Statement* stmt = stmt_table_.get_statement(a);
    Statement* stmt2 = stmt_table_.get_statement(b);
    // Invalid, not an assign stmt
    if (stmt == nullptr || stmt->get_kind() != NodeType::Assign) return ans;
    if (stmt2 == nullptr || stmt2->get_kind() != NodeType::Assign) return ans;
    std::vector<bool> visited(n, false);
    AffectsStarBFS(a, b, visited, ans, true);
  }
  return ans;
}


void PKB::UpdateVarTableWithProcs() {
  for (auto &proc : proc_table_.get_all_procedures()) {
    for (auto &var_name : *(proc->get_uses())) {
      var_table_.get_variable(var_name)->AddProcUsing(proc->get_name());
    }
    for (auto &var_name : *(proc->get_modifies())) {
      var_table_.get_variable(var_name)->AddProcModifying(proc->get_name());
    }
  }
}

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

void PKB::UsesModifiesProcessAssignNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *assign_node = dynamic_cast<AssignNode *>(node);
  Statement *assign_statement = stmt_table_.get_statement(assign_node->get_stmt_no());

  // Adding Modifies and Modifying relations for LHS
  assign_statement->AddModifies(assign_node->get_var()->get_name());
  var_table_.get_variable(assign_node->get_var()->get_name())
      ->AddStmtModifying(assign_statement->get_stmt_no());

  // Adding Uses for RHS
  for (auto &var : assign_statement->get_vars_from_expr_string()) {
    assign_statement->AddUses(var);
    var_table_.get_variable(var)->AddStmtUsing(assign_statement->get_stmt_no());
  }

  // Updating containers and procedures
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      Procedure *proc = proc_table_.get_procedure(procedure_node->get_name());
      for (auto &var : *(assign_statement->get_uses())) {
        proc->AddUses(var);
        var_table_.get_variable(var)->AddProcUsing(proc->get_name());
      }
      proc->AddModifies(assign_node->get_var()->get_name());
      var_table_.get_variable(assign_node->get_var()->get_name())->AddProcModifying(proc->get_name());
    }

    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      auto statement_node = dynamic_cast<StatementNode *>(n);
      for (auto &var : *(assign_statement->get_uses())) {
        stmt_table_.get_statement(statement_node->get_stmt_no())->AddUses(var);
        var_table_.get_variable(var)->AddStmtUsing(statement_node->get_stmt_no());
      }
      stmt_table_.get_statement(statement_node->get_stmt_no())
          ->AddModifies(assign_node->get_var()->get_name());
      var_table_.get_variable(assign_node->get_var()->get_name())
        ->AddStmtModifying(statement_node->get_stmt_no());
    }
  }
}

void PKB::UsesModifiesProcessIfNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *if_node = dynamic_cast<IfNode *>(node);
  Statement *if_statement = stmt_table_.get_statement(if_node->get_stmt_no());
  for (auto &var : if_statement->get_vars_from_expr_string()) {
    if_statement->AddUses(var);
    var_table_.get_variable(var)->AddStmtUsing(if_statement->get_stmt_no());
  }
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      Procedure *proc = proc_table_.get_procedure(procedure_node->get_name());
      for (auto &var : *(if_statement->get_uses())) {
        proc->AddUses(var);
        var_table_.get_variable(var)->AddProcUsing(proc->get_name());
      }
      for (auto &var : *(if_statement->get_modifies())) {
        proc->AddModifies(var);
        var_table_.get_variable(var)->AddProcModifying(proc->get_name());
      }
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      auto statement_node = dynamic_cast<StatementNode *>(n);
      for (auto &var : *(if_statement->get_uses())) {
        stmt_table_.get_statement(statement_node->get_stmt_no())->AddUses(var);
        var_table_.get_variable(var)->AddStmtUsing(statement_node->get_stmt_no());
      }
      for (auto &var : *(if_statement->get_modifies())) {
        stmt_table_.get_statement(statement_node->get_stmt_no())->AddModifies(var);
        var_table_.get_variable(var)->AddStmtModifying(statement_node->get_stmt_no());
      }
    }
  }
}

void PKB::UsesModifiesProcessWhileNode(Node *node,
                                       std::vector<Node *> &ancestorList) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  Statement *while_statement = stmt_table_.get_statement(while_node->get_stmt_no());
  for (auto &var : while_statement->get_vars_from_expr_string()) {
    while_statement->AddUses(var);
    var_table_.get_variable(var)->AddStmtUsing(while_statement->get_stmt_no());
  }
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      Procedure *proc = proc_table_.get_procedure(procedure_node->get_name());
      for (auto &var : *(while_statement->get_uses())) {
        proc->AddUses(var);
        var_table_.get_variable(var)->AddProcUsing(proc->get_name());
      }
      for (auto &var : *(while_statement->get_modifies())) {
        proc->AddModifies(var);
        var_table_.get_variable(var)->AddProcModifying(proc->get_name());
      }
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      auto statement_node = dynamic_cast<StatementNode *>(n);
      for (auto &var : *(while_statement->get_uses())) {
        stmt_table_.get_statement(statement_node->get_stmt_no())->AddUses(var);
        var_table_.get_variable(var)->AddStmtUsing(statement_node->get_stmt_no());
      }
      for (auto &var : *(while_statement->get_modifies())) {
        stmt_table_.get_statement(statement_node->get_stmt_no())->AddModifies(var);
        var_table_.get_variable(var)->AddStmtModifying(statement_node->get_stmt_no());
      }
    }
  }
}

// Process and store Uses relationships for the AST read node.
void PKB::UsesModifiesProcessReadNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *read_node = dynamic_cast<ReadNode *>(node);
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      proc_table_.get_procedure(procedure_node->get_name())
          ->AddModifies(read_node->get_var()->get_name());
      var_table_.get_variable(read_node->get_var()->get_name())
        ->AddProcModifying(procedure_node->get_name());
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      auto statement_node = dynamic_cast<StatementNode*>(n);
      stmt_table_.get_statement(statement_node->get_stmt_no())->AddModifies(read_node->get_var()->get_name());
      var_table_.get_variable(read_node->get_var()->get_name())->AddStmtModifying(statement_node->get_stmt_no());
    }
  }
  stmt_table_.get_statement(read_node->get_stmt_no())
      ->AddModifies(read_node->get_var()->get_name());
  var_table_.get_variable(read_node->get_var()->get_name())
      ->AddStmtModifying(read_node->get_stmt_no());
}
// Process and store Uses relationships for the AST print node.
void PKB::UsesModifiesProcessPrintNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *print_node = dynamic_cast<PrintNode *>(node);
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      proc_table_.get_procedure(procedure_node->get_name())
          ->AddUses(print_node->get_var()->get_name());
      var_table_.get_variable(print_node->get_var()->get_name())
        ->AddProcUsing(procedure_node->get_name());
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      auto statement_node = dynamic_cast<StatementNode*>(n);
      stmt_table_.get_statement(statement_node->get_stmt_no())->AddUses(print_node->get_var()->get_name());
      var_table_.get_variable(print_node->get_var()->get_name())->AddStmtUsing(statement_node->get_stmt_no());
    }
  }
  stmt_table_.get_statement(print_node->get_stmt_no())->AddUses(print_node->get_var()->get_name());
  var_table_.get_variable(print_node->get_var()->get_name())
      ->AddStmtUsing(print_node->get_stmt_no());
}

void PKB::CallsProcessCallNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *call_node = dynamic_cast<CallNode *>(node);
  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      auto *calling_procedure = proc_table_.get_procedure(procedure_node->get_name());
      calling_procedure->AddCalls(call_node->get_proc()->get_name());
      auto *called_procedure = proc_table_.get_procedure(call_node->get_proc()->get_name());
      if (called_procedure == nullptr) {
        throw PKBException("Called an undefined procedure: " + call_node->get_proc()->get_name());
      }
      called_procedure->AddCallers(procedure_node->get_name());
    }
  }
}

std::set<int> PKB::LastStmts(StatementNode *node) {
  std::set<int> ans;
  if(node->get_kind() == NodeType::If) {
    auto *if_node = dynamic_cast<IfNode*>(node);
    // Sort stmt list in ascending order,
    std::vector<StatementNode*> then_stmt_lst = if_node->get_then_stmt_lst();
    std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
      [](StatementNode *a, StatementNode *b) {
        return a->get_stmt_no() > b->get_stmt_no();
      });
    for (auto &stmt : (LastStmts(*(then_stmt_lst.begin())))) {
      ans.insert(stmt);
    }

    std::vector<StatementNode*> else_stmt_lst = if_node->get_else_stmt_lst();
    std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
      [](StatementNode *a, StatementNode *b) {
        return a->get_stmt_no() > b->get_stmt_no();
      });
    for (auto &stmt : (LastStmts(*(else_stmt_lst.begin())))) {
      ans.insert(stmt);
    }
  } else {
    ans.insert(node->get_stmt_no());
  }
  return ans;
}

void PKB::CFGProcessProcedureNode(Node *node) {
  auto *procedure_node = dynamic_cast<ProcedureNode*>(node);
  std::vector<StatementNode*> stmt_lst = procedure_node->get_stmt_lst();
  std::sort(stmt_lst.begin(), stmt_lst.end(),
    [](StatementNode *a, StatementNode *b) {
      return a->get_stmt_no() < b->get_stmt_no();
    });
  for (int i = 1; i < stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(stmt_lst[i - 1])) {
      CFGAL_[last_stmt].insert(stmt_lst[i]->get_stmt_no());
      ReverseCFGAL_[stmt_lst[i]->get_stmt_no()].insert(last_stmt);
    }
  }
}

void PKB::CFGProcessIfNode(Node *node) {
  auto *if_node = dynamic_cast<IfNode*>(node);
  std::vector<StatementNode*> then_stmt_lst = if_node->get_then_stmt_lst();
  std::sort(then_stmt_lst.begin(), then_stmt_lst.end(),
    [](StatementNode *a, StatementNode *b) {
      return a->get_stmt_no() < b->get_stmt_no();
    });
  if (then_stmt_lst.size()) {
    CFGAL_[if_node->get_stmt_no()].insert(then_stmt_lst[0]->get_stmt_no());
    ReverseCFGAL_[then_stmt_lst[0]->get_stmt_no()].insert(if_node->get_stmt_no());
  }
  for (int i = 1; i < then_stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(then_stmt_lst[i - 1])) {
      CFGAL_[last_stmt].insert(then_stmt_lst[i]->get_stmt_no());
      ReverseCFGAL_[then_stmt_lst[i]->get_stmt_no()].insert(last_stmt);
    }
  }

  std::vector<StatementNode*> else_stmt_lst = if_node->get_else_stmt_lst();
  std::sort(else_stmt_lst.begin(), else_stmt_lst.end(),
    [](StatementNode *a, StatementNode *b) {
      return a->get_stmt_no() < b->get_stmt_no();
    });
  if (else_stmt_lst.size()) {
    CFGAL_[if_node->get_stmt_no()].insert(else_stmt_lst[0]->get_stmt_no());
    ReverseCFGAL_[else_stmt_lst[0]->get_stmt_no()].insert(if_node->get_stmt_no());
  }
  for (int i = 1; i < else_stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(else_stmt_lst[i - 1])) {
      CFGAL_[last_stmt].insert(else_stmt_lst[i]->get_stmt_no());
      ReverseCFGAL_[else_stmt_lst[i]->get_stmt_no()].insert(last_stmt);
    }
  }
}

void PKB::CFGProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode*>(node);
  std::vector<StatementNode*> stmt_lst = while_node->get_stmt_list();
  std::sort(stmt_lst.begin(), stmt_lst.end(),
    [](StatementNode *a, StatementNode *b) {
      return a->get_stmt_no() < b->get_stmt_no();
    });
  if (stmt_lst.size()) {
    CFGAL_[while_node->get_stmt_no()].insert(stmt_lst[0]->get_stmt_no());
    ReverseCFGAL_[stmt_lst[0]->get_stmt_no()].insert(while_node->get_stmt_no());
  }
  for (int i = 1; i < stmt_lst.size(); i++) {
    for (auto &last_stmt : LastStmts(stmt_lst[i - 1])) {
      CFGAL_[last_stmt].insert(stmt_lst[i]->get_stmt_no());
      ReverseCFGAL_[stmt_lst[i]->get_stmt_no()].insert(last_stmt);
    }
  }
  if (stmt_lst.size()) {
    CFGAL_[stmt_lst[stmt_lst.size() - 1]->get_stmt_no()].insert(while_node->get_stmt_no());
    ReverseCFGAL_[while_node->get_stmt_no()].insert(stmt_lst[stmt_lst.size() - 1]->get_stmt_no());
  }
}
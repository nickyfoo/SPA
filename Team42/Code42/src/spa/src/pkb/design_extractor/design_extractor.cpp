#include "pkb.h"
#include <algorithm>
#include <vector>
#include <queue>
#include "ast_utils.hpp"

void PKB::ExtractEntities() {
  auto extract_variable =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::AddVariable(node, ancestor_list);
      };
  auto extract_constant =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::AddConstant(node, ancestor_list);
      };
  auto extract_statement =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::AddStatement(node, ancestor_list);
      };
  auto extract_expr_string =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        PKB::AddExprString(node, ancestor_list);
      };
  auto extract_procedure =
      [this](Node *node, std::vector<Node *> ancestor_list) {
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
  auto extract_follows_for_if_node =
      [this](Node *node) { PKB::FollowsProcessIfNode(node); };
  auto extract_follows_for_while_node =
      [this](Node *node) { PKB::FollowsProcessWhileNode(node); };
  auto extract_follows_for_procedure_node =
      [this](Node *node) { PKB::FollowsProcessProcedureNode(node); };

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
  auto extract_parents_for_if_node =
      [this](Node *node) { PKB::ParentProcessIfNode(node); };
  auto extract_parents_for_while_node =
      [this](Node *node) { PKB::ParentProcessWhileNode(node); };

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

  std::map<NodeType, std::vector<std::function<void(Node *, std::vector<Node *>)>>>
      functions = {
      {NodeType::Call, {extract_calls_for_call_node}},
  };
  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);

  proc_table_.ProcessCalls();
  proc_table_.ProcessCallsStar();
}

void PKB::ExtractCFG() {
  auto extract_cfg_for_if_node =
      [this](Node *node) { PKB::CFGProcessIfNode(node); };
  auto extract_cfg_for_while_node =
      [this](Node *node) { PKB::CFGProcessWhileNode(node); };
  auto extract_cfg_for_procedure_node =
      [this](Node *node) { PKB::CFGProcessProcedureNode(node); };

  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_cfg_for_if_node}},
      {NodeType::While, {extract_cfg_for_while_node}},
      {NodeType::Procedure, {extract_cfg_for_procedure_node}},
  };

  Visit(root_, functions);

}

void PKB::ReachabilityDFS(int start, int u, std::vector<std::vector<int>> &d,
                          std::map<int, std::set<int>> &al) {
  for (auto &v : al[u]) {
    if (d[start][v] == 0) {
      d[start][v] = 1;
      ReachabilityDFS(start, v, d, al);
    }
  }
}

void PKB::AffectsDFS(int start, int target, int u, std::string var_name,
                     std::vector<bool> &visited, std::vector<std::vector<int>> &d, bool &found) {
  if (found) return;
  Statement *stmt = stmt_table_.get_statement(start);
  for (auto &v : cfg_al_[u]) {
    Statement *stmt_v = stmt_table_.get_statement(v);
    if (visited[v]) continue;
    visited[v] = true;
    std::set<std::string> *uses = stmt_v->get_uses();
    if (stmt_v->get_kind() == NodeType::Assign && uses->find(var_name) != uses->end()) {
      d[start][v] = 1;
      if (v == target) {
        found = true;
        return;
      }
    }
    std::set<std::string> *modifies = stmt_v->get_modifies();
    if (stmt_v->get_kind() == NodeType::Assign || stmt_v->get_kind() == NodeType::Read
        || stmt_v->get_kind() == NodeType::Call) {
      if (modifies->find(var_name) != modifies->end()) continue;
    }
    AffectsDFS(start, target, v, var_name, visited, d, found);
  }
}

void PKB::AffectsStarBFS(int start, int target, std::vector<bool> &visited,
                         std::set<std::pair<int, int>> &ans, bool forward_relation) {
  std::queue<int> q;
  visited[start] = true;
  q.push(start);
  while (q.size()) {
    int u = q.front();
    q.pop();
    if (forward_relation) {
      for (auto&[a, b] : get_affects(u, kWild)) {
        if (target == kWild) ans.insert({start, b});
        else {
          if (b == target) {
            ans.insert({start, target});
            return;
          }
        }

        if (!visited[b]) {
          visited[b] = true;
          q.push(b);
        }
      }
    } else {
      for (auto&[a, b] : get_affects(kWild, u)) {
        if (target == kWild) ans.insert({a, start});
        else {
          if (a == target) {
            ans.insert({start, target});
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

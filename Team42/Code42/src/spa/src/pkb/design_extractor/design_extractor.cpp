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

  for (auto& call_stmt : stmt_table_.get_statements(NodeType::Call)) {
    int first_stmt_of_proc = proc_table_.get_procedure(call_stmt->get_called_proc_name())->get_stmt_no();
    std::set<int> visited, ans; 
    LastStmtsOfProcedure(first_stmt_of_proc, visited, ans);
    for (auto& last_stmt : ans) {
      for (auto& next_stmt : cfg_al_[call_stmt->get_stmt_no()]) {
        cfg_bip_al_[last_stmt].insert({ next_stmt, call_stmt->get_stmt_no() });
        reverse_cfg_bip_al_[next_stmt].insert({ last_stmt, call_stmt->get_stmt_no() });
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

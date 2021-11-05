#include <design_extractor.h>
#include <algorithm>
#include <utility>
#include <vector>
#include "ast_utils.hpp"

DesignExtractor::DesignExtractor(PKB *pkb, Node *program_root) {
  this->pkb_ = pkb;
  this->root_ = program_root;
}

DesignExtractor::~DesignExtractor() = default;

void DesignExtractor::ExtractDesigns() {
  ExtractEntities();
  ExtractFollows();
  ExtractParent();
  ExtractCalls();
  ExtractUsesModifies();
  ExtractCFG();
}

void DesignExtractor::ExtractEntities() {
  // Extract entity from node functions
  auto extract_variable =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        pkb_->AddVariable(node, std::move(ancestor_list));
      };
  auto extract_constant =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        pkb_->AddConstant(node, std::move(ancestor_list));
      };
  auto extract_statement =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        pkb_->AddStatement(node, std::move(ancestor_list));
      };
  auto extract_expr_string =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        pkb_->AddExprString(node, std::move(ancestor_list));
      };
  auto extract_procedure =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        pkb_->AddProcedure(node, std::move(ancestor_list));
      };

  // Map from node type to processing functions
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

  // Visits and traverse root node
  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);
  pkb_->stmt_table_.CategorizeStatements(); // Add statements to its corresponding kind table
}

void DesignExtractor::ExtractFollows() {
  // Extract follows relationship from node functions
  auto extract_follows_for_if_node =
      [this](Node *node) { DesignExtractor::FollowsProcessIfNode(node); };
  auto extract_follows_for_while_node =
      [this](Node *node) { DesignExtractor::FollowsProcessWhileNode(node); };
  auto extract_follows_for_procedure_node =
      [this](Node *node) { DesignExtractor::FollowsProcessProcedureNode(node); };

  // Map from node type to processing functions
  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_follows_for_if_node}},
      {NodeType::While, {extract_follows_for_while_node}},
      {NodeType::Procedure, {extract_follows_for_procedure_node}},
  };

  // Visits and traverse root node
  Visit(root_, functions);
  pkb_->stmt_table_.ProcessFollows();
  pkb_->stmt_table_.ProcessFollowsStar();
}

void DesignExtractor::ExtractParent() {
  // Extract parents relationship from node functions
  auto extract_parents_for_if_node =
      [this](Node *node) { DesignExtractor::ParentProcessIfNode(node); };
  auto extract_parents_for_while_node =
      [this](Node *node) { DesignExtractor::ParentProcessWhileNode(node); };

  // Map from node type to processing functions
  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_parents_for_if_node}},
      {NodeType::While, {extract_parents_for_while_node}},
  };

  // Visits and traverse root node
  Visit(root_, functions);
  pkb_->stmt_table_.ProcessParent();
  pkb_->stmt_table_.ProcessParentStar();
}

void DesignExtractor::ExtractUsesModifies() {
  // Extract uses/modifies relationship from node functions
  auto extract_uses_modifies_for_assign_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        DesignExtractor::UsesModifiesProcessAssignNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_container_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        DesignExtractor::UsesModifiesProcessContainerNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_read_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        DesignExtractor::UsesModifiesProcessReadNode(node, ancestor_list);
      };
  auto extract_uses_modifies_for_print_node =
      [this](Node *node, std::vector<Node *> ancestor_list) {
        DesignExtractor::UsesModifiesProcessPrintNode(node, ancestor_list);
      };

  // Map from node type to processing functions
  std::map<NodeType, std::vector<std::function<void(Node *, std::vector<Node *>)>>> functions = {
      {NodeType::Assign, {extract_uses_modifies_for_assign_node}},
      {NodeType::If, {extract_uses_modifies_for_container_node}},
      {NodeType::While, {extract_uses_modifies_for_container_node}},
      {NodeType::Read, {extract_uses_modifies_for_read_node}},
      {NodeType::Print, {extract_uses_modifies_for_print_node}},
  };

  // Visits and traverse root node
  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);

  pkb_->proc_table_.ProcessUsesModifiesIndirect();
  UpdateVarTableWithProcs();
  for (auto &call_statement : pkb_->get_statements(NodeType::Call)) {
    std::string called_procedure = call_statement->get_called_proc_name();
    auto uses_variables = pkb_->proc_table_.get_procedure(called_procedure)->get_uses();
    auto modifies_variables = pkb_->proc_table_.get_procedure(called_procedure)->get_modifies();
    for (auto &var_used : *uses_variables) {
      call_statement->AddUses(var_used);
      pkb_->var_table_.get_variable(var_used)->AddStmtUsing(call_statement->get_stmt_no());
    }
    for (auto &var_modified : *modifies_variables) {
      call_statement->AddModifies(var_modified);
      pkb_->var_table_.get_variable(var_modified)->AddStmtModifying(
          call_statement->get_stmt_no());
    }

    // Propagating variables in call statements to container statements.
    for (auto &parent_star : *(call_statement->get_parents_star())) {
      for (auto &var_used : *uses_variables) {
        pkb_->stmt_table_.get_statement(parent_star)->AddUses(var_used);
        pkb_->var_table_.get_variable(var_used)->AddStmtUsing(parent_star);
      }
      for (auto &var_modified : *modifies_variables) {
        pkb_->stmt_table_.get_statement(parent_star)->AddModifies(var_modified);
        pkb_->var_table_.get_variable(var_modified)->AddStmtModifying(parent_star);
      }
    }

  }
}

void DesignExtractor::ExtractCalls() {
  // Extract calls relationship from node functions
  auto extract_calls_for_call_node = [this](Node *node, std::vector<Node *> ancestor_list) {
    DesignExtractor::CallsProcessCallNode(node, ancestor_list);
  };

  // Map from node type to processing functions
  std::map<NodeType, std::vector<std::function<void(Node *, std::vector<Node *>)>>>
      functions = {
      {NodeType::Call, {extract_calls_for_call_node}},
  };

  // Visits and traverse root node
  std::vector<Node *> ancestors;
  VisitWithAncestors(root_, ancestors, functions);

  pkb_->proc_table_.ProcessCalls();
  pkb_->proc_table_.ProcessCallsStar();
}

void DesignExtractor::ExtractCFG() {
  // Extract cfg relationship from node functions
  auto extract_cfg_for_if_node =
      [this](Node *node) { DesignExtractor::CFGProcessIfNode(node); };
  auto extract_cfg_for_while_node =
      [this](Node *node) { DesignExtractor::CFGProcessWhileNode(node); };
  auto extract_cfg_for_procedure_node =
      [this](Node *node) { DesignExtractor::CFGProcessProcedureNode(node); };

  // Map from node type to processing functions
  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_cfg_for_if_node}},
      {NodeType::While, {extract_cfg_for_while_node}},
      {NodeType::Procedure, {extract_cfg_for_procedure_node}},
  };

  // Visits and traverse root node
  Visit(root_, functions);
  AddCallStacks();
}

void DesignExtractor::UpdateVarTableWithProcs() {
  for (auto &proc : pkb_->proc_table_.get_all_procedures()) {
    for (auto &var_name : *(proc->get_uses())) {
      pkb_->var_table_.get_variable(var_name)->AddProcUsing(proc->get_name());
    }
    for (auto &var_name : *(proc->get_modifies())) {
      pkb_->var_table_.get_variable(var_name)->AddProcModifying(proc->get_name());
    }
  }
}

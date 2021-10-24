#include "pkb.h"
#include <vector>
#include "ast_utils.hpp"

void PKB::UsesModifiesProcessAssignNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *assign_node = dynamic_cast<AssignNode *>(node);
  int assign_stmt_no = assign_node->get_stmt_no();
  Statement *assign_stmt = stmt_table_.get_statement(assign_stmt_no);
  std::string lhs_var = assign_node->get_var()->get_name();

  // Add LHS variable to variables modified and vice versa
  assign_stmt->AddModifies(lhs_var);
  var_table_.get_variable(lhs_var)->AddStmtModifying(assign_stmt_no);

  // Add all variables from RHS expression to the variables being used
  for (auto &var : assign_stmt->get_vars_from_expr_string()) {
    assign_stmt->AddUses(var);
    var_table_.get_variable(var)->AddStmtUsing(assign_stmt_no);
  }

  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      // Save uses/modifies variables in ancestor procedure node and vice versa
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      Procedure *pkb_proc = proc_table_.get_procedure(procedure_node->get_name());
      std::string proc_name = pkb_proc->get_name();

      for (auto &var : *(assign_stmt->get_uses())) {
        pkb_proc->AddUses(var);
        var_table_.get_variable(var)->AddProcUsing(proc_name);
      }
      pkb_proc->AddModifies(lhs_var);
      var_table_.get_variable(lhs_var)->AddProcModifying(proc_name);
    }

    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      // Save uses/modifies variables in parent if/while node and vice versa
      auto parent_node = dynamic_cast<StatementNode *>(n);
      int parent_stmt_no = parent_node->get_stmt_no();

      for (auto &var : *(assign_stmt->get_uses())) {
        stmt_table_.get_statement(parent_stmt_no)->AddUses(var);
        var_table_.get_variable(var)->AddStmtUsing(parent_stmt_no);
      }
      stmt_table_.get_statement(parent_stmt_no)->AddModifies(lhs_var);
      var_table_.get_variable(lhs_var)->AddStmtModifying(parent_stmt_no);
    }
  }
}

void PKB::UsesModifiesProcessContainerNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *cont_node = dynamic_cast<StatementNode *>(node);
  int cont_stmt_no = cont_node->get_stmt_no();
  Statement *pkb_stmt = stmt_table_.get_statement(cont_stmt_no);

  // Add all variables from if condition to the variables being used
  for (auto &var : pkb_stmt->get_vars_from_expr_string()) {
    pkb_stmt->AddUses(var);
    var_table_.get_variable(var)->AddStmtUsing(cont_stmt_no);
  }

  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      // Save uses/modifies variables in ancestor procedure node and vice versa
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      Procedure *pkb_proc = proc_table_.get_procedure(procedure_node->get_name());
      std::string proc_name = pkb_proc->get_name();

      for (auto &var : *(pkb_stmt->get_uses())) {
        pkb_proc->AddUses(var);
        var_table_.get_variable(var)->AddProcUsing(proc_name);
      }
      for (auto &var : *(pkb_stmt->get_modifies())) {
        pkb_proc->AddModifies(var);
        var_table_.get_variable(var)->AddProcModifying(proc_name);
      }
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      // Save uses/modifies variables in parent if/while node and vice versa
      auto parent_node = dynamic_cast<StatementNode *>(n);
      int parent_stmt_no = parent_node->get_stmt_no();

      for (auto &var : *(pkb_stmt->get_uses())) {
        stmt_table_.get_statement(parent_stmt_no)->AddUses(var);
        var_table_.get_variable(var)->AddStmtUsing(parent_stmt_no);
      }
      for (auto &var : *(pkb_stmt->get_modifies())) {
        stmt_table_.get_statement(parent_stmt_no)->AddModifies(var);
        var_table_.get_variable(var)->AddStmtModifying(parent_stmt_no);
      }
    }
  }
}

// Process and store Uses relationships for the AST read node.
void PKB::UsesModifiesProcessReadNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *read_node = dynamic_cast<ReadNode *>(node);
  int read_stmt_no = read_node->get_stmt_no();
  std::string read_var = read_node->get_var()->get_name();

  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      // Save read variable in ancestor procedure node and vice versa
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      std::string proc_name = procedure_node->get_name();

      proc_table_.get_procedure(proc_name)->AddModifies(read_var);
      var_table_.get_variable(read_var)->AddProcModifying(proc_name);
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      // Save read variable in parent if/while node and vice versa
      auto parent_stmt = dynamic_cast<StatementNode *>(n);
      int parent_stmt_no = parent_stmt->get_stmt_no();

      stmt_table_.get_statement(parent_stmt_no)->AddModifies(read_var);
      var_table_.get_variable(read_var)->AddStmtModifying(parent_stmt_no);
    }
  }

  stmt_table_.get_statement(read_stmt_no)->AddModifies(read_var);
  var_table_.get_variable(read_var)->AddStmtModifying(read_stmt_no);
}

// Process and store Uses relationships for the AST print node.
void PKB::UsesModifiesProcessPrintNode(Node *node, std::vector<Node *> &ancestorList) {
  auto *print_node = dynamic_cast<PrintNode *>(node);
  int print_stmt_no = print_node->get_stmt_no();
  std::string print_var = print_node->get_var()->get_name();

  for (Node *n : ancestorList) {
    if (n->get_kind() == NodeType::Procedure) {
      // Save print variable in ancestor procedure node and vice versa
      auto *procedure_node = dynamic_cast<ProcedureNode *>(n);
      std::string proc_name = procedure_node->get_name();

      proc_table_.get_procedure(proc_name)->AddUses(print_var);
      var_table_.get_variable(print_var)->AddProcUsing(proc_name);
    }
    if (n->get_kind() == NodeType::If || n->get_kind() == NodeType::While) {
      // Save read variable in parent if/while node and vice versa
      auto parent_stmt = dynamic_cast<StatementNode *>(n);
      int parent_stmt_no = parent_stmt->get_stmt_no();

      stmt_table_.get_statement(parent_stmt_no)->AddUses(print_var);
      var_table_.get_variable(print_var)->AddStmtUsing(parent_stmt_no);
    }
  }
  stmt_table_.get_statement(print_stmt_no)->AddUses(print_var);
  var_table_.get_variable(print_var)->AddStmtUsing(print_stmt_no);
}

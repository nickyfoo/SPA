#include "pkb.h"
#include <vector>
#include "ast_utils.hpp"

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
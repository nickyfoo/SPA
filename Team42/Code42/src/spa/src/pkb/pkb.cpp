#include <pkb.h>
#include <algorithm>
#include <vector>
#include "ast_utils.hpp"

PKB::PKB() = default;

PKB::~PKB() = default;

void PKB::AddProcedure(Node *node, const std::vector<Node *> &ancestor_list) {
  auto *procedure_node = dynamic_cast<ProcedureNode *>(node);
  std::string proc_name = procedure_node->get_name();
  std::vector<StatementNode *> stmt_lst = procedure_node->get_stmt_lst();
  sort(stmt_lst.begin(), stmt_lst.end(),
       [](StatementNode *a, StatementNode *b) {
         return a->get_stmt_no() < b->get_stmt_no();
       });
  int stmt_no = procedure_node->get_stmt_lst()[0]->get_stmt_no();
  proc_table_.AddProcedure(proc_name, stmt_no);
  
  // Add stmts to stmt list in Procedure
  Procedure* added_proc = proc_table_.get_procedure(proc_name);
  for (auto& stmt : stmt_lst) {
    added_proc->AddStmt(stmt->get_stmt_no());
  }
}

void PKB::AddStatement(Node *node, const std::vector<Node *> &ancestor_list) {
  stmt_table_.AddStatement(node);
  auto* stmt_node = dynamic_cast<StatementNode*>(node);
  int stmt_no = stmt_node->get_stmt_no();
  for (auto& n : ancestor_list) {
    if (n->get_kind() == NodeType::Procedure) {
      auto* parent_proc_node = dynamic_cast<ProcedureNode*>(n);
      std::string parent_proc_name = parent_proc_node->get_name();
      stmt_table_.get_statement(stmt_no)->set_parent_proc(parent_proc_name);
      break;
    }
  }
}

void PKB::AddExprString(Node *node, const std::vector<Node *> &ancestor_list) {
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
      default:
        break;
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
      default:
        break;
    }
  }
}

void PKB::AddVariable(Node *node, const std::vector<Node *> &ancestor_list) {
  auto *identifier_node = dynamic_cast<IdentifierNode *>(node);
  if (ancestor_list.back()->get_kind() != NodeType::Call) {
    var_table_.AddVariable(identifier_node->get_name());
  }
}

void PKB::AddConstant(Node *node, const std::vector<Node *> &ancestor_list) {
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

std::map<int, std::set<int>> *PKB::get_cfg_al() {
  return &cfg_al_;
}

std::map<int, std::set<int>> *PKB::get_reverse_cfg_al() {
  return &reverse_cfg_al_;
}

std::map<int, std::set<std::pair<int, int>>> *PKB::get_cfg_bip_al() {
  return &cfg_bip_al_;
}
std::map<int, std::set<std::pair<int, int>>> *PKB::get_reverse_cfg_bip_al() {
  return &reverse_cfg_bip_al_;
}

bool PKB::TestAssignmentPattern(Statement *statement,
                                const std::string &pattern,
                                bool is_partial_match) {
  return pattern_manager_.TestAssignmentPattern(statement, pattern, is_partial_match);
}

bool PKB::TestIfWhilePattern(Statement *stmt, const std::string &variable) {
  return pattern_manager_.TestIfWhilePattern(stmt, variable);
}


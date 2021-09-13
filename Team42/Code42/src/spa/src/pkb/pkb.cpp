#include <vector>
#include "pkb.h"
#include <algorithm>
#include "ast_utils.hpp"

PKB::PKB(Node *programRoot) {
  this->root_ = programRoot;
  Initialise();
}

PKB::~PKB() = default;

void PKB::AddProcedure(Node *node) {
  auto *procedure_node = dynamic_cast<ProcedureNode *>(node);
  proc_table_.AddProcedure(procedure_node->get_name());
}

void PKB::AddStatement(Node *node) {
  stmt_table_.AddStatement(node);
}

void PKB::AddExprString(Node *node) {
  if (node->get_kind() == NodeType::Assign) {
    auto *assign_node = dynamic_cast<AssignNode *>(node);
    switch (assign_node->get_kind()) {
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
        // TODO: might throw an error here
        break;
    }
  }
}

void PKB::AddVariable(Node *node) {
  auto *identifier_node = dynamic_cast<IdentifierNode *>(node);
  var_table_.AddVariable(identifier_node->get_name());
}

void PKB::AddConstant(Node *node) {
  auto *constant_node = dynamic_cast<ConstantNode *>(node);
  const_table_.AddConstant(std::stoi(constant_node->get_value()));
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

bool PKB::TestAssignmentPattern(Statement *statement, std::string pattern, bool is_partial_match) {
  return PatternManager::TestAssignmentPattern(statement, pattern, is_partial_match);
}

void PKB::PrintStatements() {
  stmt_table_.PrintStatements();
}

void PKB::Initialise() {
  ExtractEntities();
  GetFollows();
  GetParent();
}

void PKB::ExtractEntities() {
  auto extract_variable = [this](Node *node) { PKB::AddVariable(node); };
  auto extract_constant = [this](Node *node) { PKB::AddConstant(node); };
  auto extract_statement = [this](Node *node) { PKB::AddStatement(node); };
  auto extract_expr_string = [this](Node *node) { PKB::AddExprString(node); };
  auto extract_procedure = [this](Node *node) { PKB::AddProcedure(node); };

  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::Identifier, {extract_variable}},
      {NodeType::Constant, {extract_constant}},
      {NodeType::Assign, {extract_statement, extract_expr_string}},
      {NodeType::If, {extract_statement}},
      {NodeType::While, {extract_statement}},
      {NodeType::Read, {extract_statement}},
      {NodeType::Print, {extract_statement}},
      {NodeType::Call, {extract_statement}},
      {NodeType::Procedure, {extract_procedure}},
  };

  Visit(root_, functions);

  stmt_table_.PrintStatements();
  proc_table_.PrintProcedures();
  var_table_.PrintVariables();
  const_table_.PrintConstants();
}

void PKB::GetFollows() {
  auto extract_follows_for_if_node = [this](Node *node) { PKB::FollowsProcessIfNode(node); };
  auto extract_follows_for_while_node = [this](Node *node) { PKB::FollowsProcessWhileNode(node); };
  auto extract_follows_for_procedure_node = [this](Node *node) { PKB::FollowsProcessProcedureNode(node); };

  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {extract_follows_for_if_node}},
      {NodeType::While, {extract_follows_for_while_node}},
      {NodeType::Procedure, {extract_follows_for_procedure_node}},
  };
  Visit(root_, functions);
  stmt_table_.ProcessFollows();
  stmt_table_.ProcessFollowsStar();
}

void PKB::GetParent() {
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

  for (StatementNode *n : if_node->get_then_stmt_lst()) {
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
  // TODO: Line numbers are stored and sorted at the moment
  //  as it is not clear how it statement list is organised / sorted
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
    // TODO: Check and throw error if NULL
    stmt_table_.get_statement(n->get_stmt_no())->AddParent(if_statement->get_stmt_no());
  }

  for (StatementNode *n : if_node->get_then_stmt_lst()) {
    if_statement->AddChild(n->get_stmt_no());
    // TODO: Check and throw error if NULL
    stmt_table_.get_statement(n->get_stmt_no())->AddParent(if_statement->get_stmt_no());
  }
}

void PKB::ParentProcessWhileNode(Node *node) {
  auto *while_node = dynamic_cast<WhileNode *>(node);
  Statement *while_statement = stmt_table_.get_statement(while_node->get_stmt_no());
  for (StatementNode *n : while_node->get_stmt_list()) {
    while_statement->AddChild(n->get_stmt_no());
    stmt_table_.get_statement(n->get_stmt_no())->AddParent(while_statement->get_stmt_no()); //might want to do error checking here if NULL
  }
}
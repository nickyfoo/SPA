#include <vector>
#include "pkb.h"
#include "ast_utils.hpp"

PKB::PKB(Node *programRoot) {
  this->root_ = programRoot;
  Initialise();
}

PKB::~PKB() = default;

void PKB::AddProcedure(Node *node) {
  auto *procedure_node = (ProcedureNode *) node;
  proc_table_.AddProcedure(procedure_node->get_name());
}

void PKB::AddStatement(Node *node) {
  stmt_table_.AddStatement(node);
}

void PKB::AddExprString(Node *node) {
  if (node->get_kind() == NodeType::Assign) {
    auto *assign_node = (AssignNode *) node;
    switch (assign_node->get_kind()) {
      case NodeType::Expression: {
        auto *expression_node = (ExpressionNode *) assign_node->expr();
        std::string expr_string = expression_node->get_expr_string();
        stmt_table_.get_statement(assign_node->get_stmt_no())->set_expr_string(expr_string);
        break;
      }
      case NodeType::Constant: {
        auto *constant_node = (ConstantNode *) assign_node->expr();
        std::string expr_string = constant_node->get_expr_string();
        stmt_table_.get_statement(assign_node->get_stmt_no())->set_expr_string(expr_string);
        break;
      }
      case NodeType::Identifier: {
        auto *identifier_node = (IdentifierNode *) assign_node->expr();
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
  auto *identifier_node = (IdentifierNode *) node;
  var_table_.AddVariable(identifier_node->get_name());
}

void PKB::AddConstant(Node *node) {
  auto *constant_node = (ConstantNode *) node;
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
  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::Identifier, {[this](Node *node) { PKB::AddVariable(node); }}},
      {NodeType::Constant, {[this](Node *node) { PKB::AddConstant(node); }}},
      {NodeType::Assign, {
          [this](Node *node) { PKB::AddStatement(node); },
          [this](Node *node) { PKB::AddExprString(node); }}},
      {NodeType::If, {[this](Node *node) { PKB::AddStatement(node); }}},
      {NodeType::While, {[this](Node *node) { PKB::AddStatement(node); }}},
      {NodeType::Read, {[this](Node *node) { PKB::AddStatement(node); }}},
      {NodeType::Print, {[this](Node *node) { PKB::AddStatement(node); }}},
      {NodeType::Call, {[this](Node *node) { PKB::AddStatement(node); }}},
      {NodeType::Procedure, {[this](Node *node) { PKB::AddProcedure(node); }}},
  };

  Visit(root_, functions);

  stmt_table_.PrintStatements();
  proc_table_.PrintProcedures();
  var_table_.PrintVariables();
  const_table_.PrintConstants();
}

void PKB::GetFollows() {
  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {[this](Node *node) { PKB::FollowsProcessIfNode(node); }}},
      {NodeType::While, {[this](Node *node) { PKB::FollowsProcessWhileNode(node); }}},
      {NodeType::Procedure, {[this](Node *node) { PKB::FollowsProcessProcedureNode(node); }}},
  };
  Visit(root_, functions);
  stmt_table_.ProcessFollows();
  stmt_table_.ProcessFollowsStar();
}

void PKB::GetParent() {
  std::map<NodeType, std::vector<std::function<void(Node *)>>> functions = {
      {NodeType::If, {[this](Node *node) { PKB::ParentProcessIfNode(node); }}},
      {NodeType::While, {[this](Node *node) { PKB::ParentProcessWhileNode(node); }}},
  };
  Visit(root_, functions);
  stmt_table_.ProcessParent();
  stmt_table_.ProcessParentStar();
}

void PKB::FollowsProcessProcedureNode(Node *node) {
  auto *procedure_node = (ProcedureNode *) node;
  std::vector<int> line_no;

  for (Node *n : procedure_node->get_stmt_lst()) {
    line_no.push_back(Statement::get_stmt_no(n));
  }
  sort(line_no.begin(), line_no.end());
  for (int i = 1; i < line_no.size(); i++) {
    stmt_table_.get_statement(line_no[i - 1])->AddFollower(line_no[i]);
    stmt_table_.get_statement(line_no[i])->AddFollowee(line_no[i - 1]);
  }
}

void PKB::FollowsProcessIfNode(Node *node) {
  auto *if_node = (IfNode *) node;
  std::vector<int> then_line_nos, else_line_nos;

  for (Node *n : if_node->get_then_stmt_lst()) {
    then_line_nos.push_back(Statement::get_stmt_no(n));
  }
  sort(then_line_nos.begin(), then_line_nos.end());
  for (int i = 1; i < then_line_nos.size(); i++) {
    stmt_table_.get_statement(then_line_nos[i - 1])->AddFollower(then_line_nos[i]);
    stmt_table_.get_statement(then_line_nos[i])->AddFollowee(then_line_nos[i - 1]);
  }

  for (Node *n : if_node->get_then_stmt_lst()) {
    else_line_nos.push_back(Statement::get_stmt_no(n));
  }
  sort(else_line_nos.begin(), else_line_nos.end());
  for (int i = 1; i < else_line_nos.size(); i++) {
    stmt_table_.get_statement(else_line_nos[i - 1])->AddFollower(else_line_nos[i]);
    stmt_table_.get_statement(else_line_nos[i])->AddFollowee(else_line_nos[i - 1]);
  }
}

void PKB::FollowsProcessWhileNode(Node *node) {
  auto *while_node = (WhileNode *) node;
  // TODO: Line numbers are stored and sorted at the moment
  //  as it is not clear how it statement list is organised / sorted
  std::vector<int> line_nos;
  for (Node *n : while_node->get_stmt_list()) {
    line_nos.push_back(Statement::get_stmt_no(n));
  }
  sort(line_nos.begin(), line_nos.end());
  for (int i = 1; i < line_nos.size(); i++) {
    stmt_table_.get_statement(line_nos[i - 1])->AddFollower(line_nos[i]);
    stmt_table_.get_statement(line_nos[i])->AddFollowee(line_nos[i - 1]);
  }
}

void PKB::ParentProcessIfNode(Node *node) {
  auto *if_node = (IfNode *) node;
  Statement *if_statement = stmt_table_.get_statement(if_node->get_stmt_no());
  for (Node *n : if_node->get_then_stmt_lst()) {
    if_statement->AddChild(Statement::get_stmt_no(n));
    // TODO: Check and throw error if NULL
    stmt_table_.get_statement(Statement::get_stmt_no(n))->AddParent(if_statement->get_stmt_no());
  }

  for (Node *n : if_node->get_then_stmt_lst()) {
    if_statement->AddChild(Statement::get_stmt_no(n));
    // TODO: Check and throw error if NULL
    stmt_table_.get_statement(Statement::get_stmt_no(n))->AddParent(if_statement->get_stmt_no());
  }
}

void PKB::ParentProcessWhileNode(Node *node) {
  auto *while_node = (WhileNode *) node;
  Statement *while_statement = stmt_table_.get_statement(while_node->get_stmt_no());
  for (Node *n : while_node->get_stmt_list()) {
    while_statement->AddChild(Statement::get_stmt_no(n));
    stmt_table_.get_statement(Statement::get_stmt_no(n))->AddParent(while_statement->get_stmt_no()); //might want to do error checking here if NULL
  }
}
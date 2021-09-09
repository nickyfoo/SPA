#include <vector>
#include "PKB.h"

PKB::PKB(ast::Node *programRoot) {
  this->root_ = programRoot;
}

PKB::~PKB() {}

void PKB::Initialise() {
  ExtractEntities();
  GetFollows();
  GetParent();
}

void PKB::AddProcedure(ast::Node *node) {
  auto *procedure_node = (ast::ProcedureNode *) node;
  proc_table_.AddProcedure(procedure_node->name);
}

void PKB::AddStatement(ast::Node *node) {
  stmt_table_.AddStatement(node);
}

void PKB::AddExprString(ast::Node *node) {
  if (node->kind == ast::Assign) {
    auto *assign_node = (ast::AssignNode *) node;
    stmt_table_.GetStatement(assign_node->stmtNo)->SetExprString(assign_node->expr->exprString);
  }
}

void PKB::AddVariable(ast::Node *node) {
  auto *identifier_node = (ast::IdentifierNode *) node;
  var_table_.AddVariable(identifier_node->name);
}

void PKB::AddConstant(ast::Node *node) {
  auto *constant_node = (ast::ConstantNode *) node;
  const_table_.AddConstant(std::stoi(constant_node->value));
}

std::vector<Procedure *> PKB::GetAllProcedures() {
  return proc_table_.GetAllProcedures();
}

int PKB::GetNumStatements() {
  return stmt_table_.GetNumStatements();
}

std::vector<Statement *> PKB::GetAllStatements() {
  return stmt_table_.GetAllStatements();
}

std::vector<Statement *> PKB::GetStatements(ast::Kind type) {
  return stmt_table_.GetStatements(type);
}

Statement *PKB::GetStatement(int line_no) {
  return stmt_table_.GetStatement(line_no);
}

std::vector<Variable *> PKB::GetAllVariables() {
  return var_table_.GetAllVariables();
}

void PKB::followsProcessProcedureNode(ast::Node *node) {
  auto *procedure_node = (ast::ProcedureNode *) node;
  std::vector<int> line_no;

  for (ast::Node *n : procedure_node->stmtLst) {
    line_no.push_back(Statement::GetStmtNo(n));
  }
  sort(line_no.begin(), line_no.end());
  for (int i = 1; i < line_no.size(); i++) {
    stmt_table_.GetStatement(line_no[i - 1])->AddFollower(line_no[i]);
    stmt_table_.GetStatement(line_no[i])->AddFollowee(line_no[i - 1]);
  }
}

void PKB::followsProcessIfNode(ast::Node *node) {
  auto *if_node = (ast::IfNode *) node;
  std::vector<int> then_line_no, else_line_no;

  for (ast::Node *n : if_node->thenStmtLst) {
    then_line_no.push_back(Statement::GetStmtNo(n));
  }
  sort(then_line_no.begin(), then_line_no.end());
  for (int i = 1; i < then_line_no.size(); i++) {
    stmt_table_.GetStatement(then_line_no[i - 1])->AddFollower(then_line_no[i]);
    stmt_table_.GetStatement(then_line_no[i])->AddFollowee(then_line_no[i - 1]);
  }

  for (ast::Node *n : if_node->thenStmtLst) {
    else_line_no.push_back(Statement::GetStmtNo(n));
  }
  sort(else_line_no.begin(), else_line_no.end());
  for (int i = 1; i < else_line_no.size(); i++) {
    stmt_table_.GetStatement(else_line_no[i - 1])->AddFollower(else_line_no[i]);
    stmt_table_.GetStatement(else_line_no[i])->AddFollowee(else_line_no[i - 1]);
  }

}

void PKB::followsProcessWhileNode(ast::Node *node) {
  auto *while_node = (ast::WhileNode *) node;
  // TODO: Line numbers are stored and sorted at the moment
  //  as it is not clear how it statement list is organised / sorted
  std::vector<int> line_no;
  for (ast::Node *n : while_node->stmtLst) {
    line_no.push_back(Statement::GetStmtNo(n));
  }
  sort(line_no.begin(), line_no.end());
  for (int i = 1; i < line_no.size(); i++) {
    stmt_table_.GetStatement(line_no[i - 1])->AddFollower(line_no[i]);
    stmt_table_.GetStatement(line_no[i])->AddFollowee(line_no[i - 1]);
  }
}

void PKB::parentProcessIfNode(ast::Node *node) {
  auto *if_node = (ast::IfNode *) node;
  Statement *ifStatement = stmt_table_.GetStatement(if_node->stmtNo);
  for (ast::Node *n : if_node->thenStmtLst) {
    ifStatement->AddChild(Statement::GetStmtNo(n));
    stmt_table_.GetStatement(Statement::GetStmtNo(n))->AddParent(ifStatement->GetStmtNo()); //might want to do error checking here if NULL
  }

  for (ast::Node *n : if_node->thenStmtLst) {
    ifStatement->AddChild(Statement::GetStmtNo(n));
    stmt_table_.GetStatement(Statement::GetStmtNo(n))->AddParent(ifStatement->GetStmtNo()); //might want to do error checking here if NULL
  }
}

void PKB::parentProcessWhileNode(ast::Node *node) {
  auto *while_node = (ast::WhileNode *) node;
  Statement *while_statement = stmt_table_.GetStatement(while_node->stmtNo);
  for (ast::Node *n : while_node->stmtLst) {
    while_statement->AddChild(Statement::GetStmtNo(n));
    stmt_table_.GetStatement(Statement::GetStmtNo(n))->AddParent(while_statement->GetStmtNo()); //might want to do error checking here if NULL
  }
}

void PKB::PrintStatements() {
  stmt_table_.PrintStatements();
}

void PKB::ExtractEntities() {
  std::map<ast::Kind, std::vector<std::function<void(ast::Node *)>>> functions;

  functions[ast::Identifier].push_back([this](ast::Node *node) { PKB::AddVariable(node); });
  for (auto it = begin(StmtTable::kValidStmts); it != end(StmtTable::kValidStmts); ++it) {
    functions[*it].push_back([this](ast::Node *node) { PKB::AddStatement(node); });
  }
  functions[ast::Constant].push_back([this](ast::Node *node) { PKB::AddConstant(node); });
  functions[ast::Assign].push_back([this](ast::Node *node) { PKB::AddExprString(node); });
  functions[ast::Procedure].push_back([this](ast::Node *node) { PKB::AddProcedure(node); });

  ast::visit(root_, functions);

  stmt_table_.PrintStatements();
  proc_table_.PrintProcedures();
  var_table_.PrintVariables();
  const_table_.PrintConstants();
}

void PKB::GetFollows() {
  std::map<ast::Kind, std::vector<std::function<void(ast::Node *)>>> functions;
  functions[ast::Procedure].push_back([this](ast::Node *node) { PKB::followsProcessProcedureNode(node); });
  functions[ast::If].push_back([this](ast::Node *node) { PKB::followsProcessIfNode(node); });
  functions[ast::While].push_back([this](ast::Node *node) { PKB::followsProcessWhileNode(node); });
  ast::visit(root_, functions);
  stmt_table_.ProcessFollows();
  stmt_table_.ProcessFollowsStar();
}

void PKB::GetParent() {
  std::map<ast::Kind, std::vector<std::function<void(ast::Node *)>>> functions;
  functions[ast::If].push_back([this](ast::Node *node) { PKB::parentProcessIfNode(node); });
  functions[ast::While].push_back([this](ast::Node *node) { PKB::parentProcessWhileNode(node); });
  ast::visit(root_, functions);
  stmt_table_.ProcessParent();
  stmt_table_.ProcessParentStar();
}
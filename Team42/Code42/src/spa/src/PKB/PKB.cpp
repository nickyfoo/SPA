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
  ast::ProcedureNode *castedProcedureNode = (ast::ProcedureNode *) node;
  proc_table_.AddProcedure(castedProcedureNode->name);
}

void PKB::AddStatement(ast::Node *node) {
  stmt_table_.AddStatement(node);
}

void PKB::AddExprString(ast::Node *node) {
  if (node->kind == ast::Assign) {
    ast::AssignNode *castedAssignNode{static_cast<ast::AssignNode *>(node)};
    stmt_table_.GetStatement(castedAssignNode->stmtNo)->SetExprString(castedAssignNode->expr->exprString);
  }
}

void PKB::AddVariable(ast::Node *node) {
  ast::IdentifierNode *castedIdentifierNode = (ast::IdentifierNode *) node;
  var_table_.AddVariable(castedIdentifierNode->name);
}

void PKB::AddConstant(ast::Node *node) {
  ast::ConstantNode *castedConstantNode = (ast::ConstantNode *) node;
  const_table_.AddConstant(std::stoi(castedConstantNode->value));
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
  ast::ProcedureNode *castedProcedureNode = (ast::ProcedureNode *) node;
  std::vector<int> lineNumbers;

  for (ast::Node *n : castedProcedureNode->stmtLst) {
    lineNumbers.push_back(Statement::GetStmtNo(n));
  }
  sort(lineNumbers.begin(), lineNumbers.end());
  for (int i = 1; i < lineNumbers.size(); i++) {
    stmt_table_.GetStatement(lineNumbers[i - 1])->AddFollower(lineNumbers[i]);
    stmt_table_.GetStatement(lineNumbers[i])->AddFollowee(lineNumbers[i - 1]);
  }
}

void PKB::followsProcessIfNode(ast::Node *node) {
  ast::IfNode *castedIfNode = (ast::IfNode *) node;
  std::vector<int> thenLineNumbers, elseLineNumbers;

  for (ast::Node *n : castedIfNode->thenStmtLst) {
    thenLineNumbers.push_back(Statement::GetStmtNo(n));
  }
  sort(thenLineNumbers.begin(), thenLineNumbers.end());
  for (int i = 1; i < thenLineNumbers.size(); i++) {
    stmt_table_.GetStatement(thenLineNumbers[i - 1])->AddFollower(thenLineNumbers[i]);
    stmt_table_.GetStatement(thenLineNumbers[i])->AddFollowee(thenLineNumbers[i - 1]);
  }

  for (ast::Node *n : castedIfNode->thenStmtLst) {
    elseLineNumbers.push_back(Statement::GetStmtNo(n));
  }
  sort(elseLineNumbers.begin(), elseLineNumbers.end());
  for (int i = 1; i < elseLineNumbers.size(); i++) {
    stmt_table_.GetStatement(elseLineNumbers[i - 1])->AddFollower(elseLineNumbers[i]);
    stmt_table_.GetStatement(elseLineNumbers[i])->AddFollowee(elseLineNumbers[i - 1]);
  }

}

void PKB::followsProcessWhileNode(ast::Node *node) {
  ast::WhileNode *castedWhileNode = (ast::WhileNode *) node;
  // TODO: Line numbers are stored and sorted at the moment
  //  as it is not clear how it statement list is organised / sorted
  std::vector<int> lineNumbers;
  for (ast::Node *n : castedWhileNode->stmtLst) {
    lineNumbers.push_back(Statement::GetStmtNo(n));
  }
  sort(lineNumbers.begin(), lineNumbers.end());
  for (int i = 1; i < lineNumbers.size(); i++) {
    stmt_table_.GetStatement(lineNumbers[i - 1])->AddFollower(lineNumbers[i]);
    stmt_table_.GetStatement(lineNumbers[i])->AddFollowee(lineNumbers[i - 1]);
  }
}

void PKB::parentProcessIfNode(ast::Node *node) {
  ast::IfNode *castedIfNode = (ast::IfNode *) node;
  Statement *ifStatement = stmt_table_.GetStatement(castedIfNode->stmtNo);
  for (ast::Node *n : castedIfNode->thenStmtLst) {
    ifStatement->AddChild(Statement::GetStmtNo(n));
    stmt_table_.GetStatement(Statement::GetStmtNo(n))->AddParent(ifStatement->GetStmtNo()); //might want to do error checking here if NULL
  }

  for (ast::Node *n : castedIfNode->thenStmtLst) {
    ifStatement->AddChild(Statement::GetStmtNo(n));
    stmt_table_.GetStatement(Statement::GetStmtNo(n))->AddParent(ifStatement->GetStmtNo()); //might want to do error checking here if NULL
  }
}

void PKB::parentProcessWhileNode(ast::Node *node) {
  ast::WhileNode *castedWhileNode = (ast::WhileNode *) node;
  Statement *whileStatement = stmt_table_.GetStatement(castedWhileNode->stmtNo);
  for (ast::Node *n : castedWhileNode->stmtLst) {
    whileStatement->AddChild(Statement::GetStmtNo(n));
    stmt_table_.GetStatement(Statement::GetStmtNo(n))->AddParent(whileStatement->GetStmtNo()); //might want to do error checking here if NULL
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
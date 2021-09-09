#pragma once

#include <string>
#include <vector>

enum NodeType {
  Identifier,
  Constant,
  Expression,
  RelExpression,
  CondExpression,
  Assign,
  If,
  While,
  Read,
  Print,
  Call,
  Procedure,
  Program
};

enum ExprOp { Plus, Minus, Times, Divide, Modulo };

enum RelExprOp { Gt, Gte, Lt, Lte, Eq, Neq };

enum CondExprOp { And, Or, Not };

struct LocInfo {
  int line_no;
  int col_no;
};

class Node {
 private:
  int line_no_;
  int col_no_;

 protected:
  Node(LocInfo loc);

 public:
  virtual NodeType kind() = 0;
  int line_no();
  int col_no();
};

class StatementNode : public Node {
 private:
  int stmt_no_;

 protected:
  StatementNode(int stmt_no, LocInfo loc);

 public:
  virtual NodeType kind() = 0;
  int stmt_no();
};

class IdentifierNode : public Node {
 private:
  std::string name_;

 public:
  IdentifierNode(std::string name, LocInfo loc);
  NodeType kind();
  std::string name();
};

class ConstantNode : public Node {
 private:
  std::string value_;

 public:
  ConstantNode(std::string value, LocInfo loc);
  NodeType kind();
  std::string value();
};

class ExpressionNode : public Node {
 private:
  ExprOp op_;
  Node *left_;               // must be Expression, Constant or Identifier
  Node *right_;              // must be Expression, Constant or Identifier
  std::string expr_string_;  // converted to postfix notation

 public:
  ExpressionNode(ExprOp op, Node *left, Node *right, std::string expr_string, LocInfo loc);
  NodeType kind();
  ExprOp op();
  Node *left();
  Node *right();
  std::string expr_string();
};

class RelExpressionNode : public Node {
 private:
  RelExprOp op_;
  Node *left_;   // must be Expresesion, Constant or Identifier
  Node *right_;  // must be Expression, Constant or Identifier

 public:
  RelExpressionNode(RelExprOp op, Node *left, Node *right, LocInfo loc);
  NodeType kind();
  RelExprOp op();
  Node *left();
  Node *right();
};

class CondExpressionNode : public Node {
 private:
  CondExprOp op_;
  Node *left_;   // must be RelExpression or CondExpression
  Node *right_;  // must be RelExpression or CondExpression

 public:
  CondExpressionNode(CondExprOp op, Node *left, Node *right, LocInfo loc);
  NodeType kind();
  CondExprOp op();
  Node *left();
  Node *right();
};

class AssignNode : public StatementNode {
 private:
  IdentifierNode *var_;
  Node *expr_;  // must be Expression, Constant or Identifier

 public:
  AssignNode(IdentifierNode *var, Node *expr, int stmt_no, LocInfo loc);
  NodeType kind();
  IdentifierNode *var();
  Node *expr();
};

class IfNode : public StatementNode {
 private:
  Node *cond_;  // must be RelExprNode or CondExprNode
  std::vector<StatementNode *> then_stmt_lst_;
  std::vector<StatementNode *> else_stmt_lst_;

 public:
  IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
         std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc);
  NodeType kind();
  Node *cond();
  std::vector<StatementNode *> then_stmt_lst();
  std::vector<StatementNode *> else_stmt_lst();
};

class WhileNode : public StatementNode {
 private:
  Node *cond_;  // must be RelExprNode or CondExprNode
  std::vector<StatementNode *> stmt_lst_;

 public:
  WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst, int stmt_no, LocInfo loc);
  NodeType kind();
  Node *cond();
  std::vector<StatementNode *> stmt_list();
};

class ReadNode : public StatementNode {
 private:
  IdentifierNode *var_;

 public:
  ReadNode(IdentifierNode *var, int stmt_no, LocInfo loc);
  NodeType kind();
  IdentifierNode *var();
};

class PrintNode : public StatementNode {
 private:
  IdentifierNode *var_;

 public:
  PrintNode(IdentifierNode *var, int stmt_no, LocInfo loc);
  NodeType kind();
  IdentifierNode *var();
};

class CallNode : public StatementNode {
 private:
  IdentifierNode *proc_;

 public:
  CallNode(IdentifierNode *proc, int stmt_no, LocInfo loc);
  NodeType kind();
  IdentifierNode *proc();
};

class ProcedureNode : public Node {
 private:
  std::string name_;
  std::vector<StatementNode *> stmt_lst_;

 public:
  ProcedureNode(std::string name, std::vector<StatementNode *> stmt_lst, LocInfo loc);
  NodeType kind();
  std::string name();
  std::vector<StatementNode *> stmt_lst();
};

class ProgramNode : public Node {
 private:
  std::vector<ProcedureNode *> procedures_;

 public:
  ProgramNode(std::vector<ProcedureNode *> procedures, LocInfo loc);
  NodeType kind();
  std::vector<ProcedureNode *> procedures();
};

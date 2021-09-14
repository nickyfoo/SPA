#pragma once

#include <string>
#include <vector>

enum class NodeType {
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

enum class ExprOp { Plus, Minus, Times, Divide, Modulo };

enum class RelExprOp { Gt, Gte, Lt, Lte, Eq, Neq };

enum class CondExprOp { And, Or, Not };

struct LocInfo {
  int line_no;
  int col_no;
};

class Node {
 public:
  virtual NodeType get_kind() = 0;
  int get_line_no();
  int get_col_no();

 protected:
  Node(LocInfo loc);

 private:
  int line_no_;
  int col_no_;
};

class StatementNode : public Node {
 public:
  virtual NodeType get_kind() = 0;
  int get_stmt_no();

 protected:
  StatementNode(int stmt_no, LocInfo loc);

 private:
  int stmt_no_;
};

class IdentifierNode : public Node {
 public:
  IdentifierNode(std::string name, LocInfo loc);
  NodeType get_kind();
  std::string get_name();
  std::string get_expr_string();

 private:
  std::string name_;
};

class ConstantNode : public Node {
 public:
  ConstantNode(std::string value, LocInfo loc);
  NodeType get_kind();
  std::string get_value();
  std::string get_expr_string();

 private:
  std::string value_;
};

class ExpressionNode : public Node {
 public:
  ExpressionNode(ExprOp op, Node *left, Node *right, std::string expr_string, LocInfo loc);
  NodeType get_kind();
  ExprOp get_op();
  Node *get_left();
  Node *get_right();
  std::string get_expr_string();

 private:
  ExprOp op_;
  Node *left_;               // must be Expression, Constant or Identifier
  Node *right_;              // must be Expression, Constant or Identifier
  std::string expr_string_;  // converted to postfix notation
};

class RelExpressionNode : public Node {
 public:
  RelExpressionNode(RelExprOp op, Node *left, Node *right, LocInfo loc);
  NodeType get_kind();
  RelExprOp get_op();
  Node *get_left();
  Node *get_right();

 private:
  RelExprOp op_;
  Node *left_;   // must be Expresesion, Constant or Identifier
  Node *right_;  // must be Expression, Constant or Identifier
};

class CondExpressionNode : public Node {
 public:
  CondExpressionNode(CondExprOp op, Node *left, Node *right, std::string expr_string, LocInfo loc);
  NodeType get_kind();
  CondExprOp get_op();
  Node *get_left();
  Node *get_right();
  std::string get_expr_string();

 private:
  CondExprOp op_;
  Node *left_;   // must be RelExpression or CondExpression
  Node *right_;  // must be RelExpression or CondExpression
  std::string expr_string_;  // converted to postfix notation
};

class AssignNode : public StatementNode {
 public:
  AssignNode(IdentifierNode *var, Node *expr, int stmt_no, LocInfo loc);
  NodeType get_kind();
  IdentifierNode *get_var();
  Node *expr();

 private:
  IdentifierNode *var_;
  Node *expr_;  // must be Expression, Constant or Identifier
};

class IfNode : public StatementNode {
 public:
  IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
         std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc);
  NodeType get_kind();
  Node *get_cond();
  std::vector<StatementNode *> get_then_stmt_lst();
  std::vector<StatementNode *> get_else_stmt_lst();

 private:
  Node *cond_;  // must be RelExprNode or CondExprNode
  std::vector<StatementNode *> then_stmt_lst_;
  std::vector<StatementNode *> else_stmt_lst_;
};

class WhileNode : public StatementNode {
 public:
  WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst, int stmt_no, LocInfo loc);
  NodeType get_kind();
  Node *get_cond();
  std::vector<StatementNode *> get_stmt_list();

 private:
  Node *cond_;  // must be RelExprNode or CondExprNode
  std::vector<StatementNode *> stmt_lst_;
};

class ReadNode : public StatementNode {
 public:
  ReadNode(IdentifierNode *var, int stmt_no, LocInfo loc);
  NodeType get_kind();
  IdentifierNode *get_var();

 private:
  IdentifierNode *var_;
};

class PrintNode : public StatementNode {
 public:
  PrintNode(IdentifierNode *var, int stmt_no, LocInfo loc);
  NodeType get_kind();
  IdentifierNode *get_var();

 private:
  IdentifierNode *var_;
};

class CallNode : public StatementNode {
 public:
  CallNode(IdentifierNode *proc, int stmt_no, LocInfo loc);
  NodeType get_kind();
  IdentifierNode *get_proc();

 private:
  IdentifierNode *proc_;
};

class ProcedureNode : public Node {
 public:
  ProcedureNode(std::string name, std::vector<StatementNode *> stmt_lst, LocInfo loc);
  NodeType get_kind();
  std::string get_name();
  std::vector<StatementNode *> get_stmt_lst();

 private:
  std::string name_;
  std::vector<StatementNode *> stmt_lst_;
};

class ProgramNode : public Node {
 public:
  ProgramNode(std::vector<ProcedureNode *> procedures, LocInfo loc);
  NodeType get_kind();
  std::vector<ProcedureNode *> get_procedures();

 private:
  std::vector<ProcedureNode *> procedures_;
};


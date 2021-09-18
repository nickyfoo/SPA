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

class AssignNodeExpr : public virtual Node {
 public:
  virtual NodeType get_kind() = 0;
  virtual std::string get_expr_string() = 0;
};

class ExpressionNodeChild : public virtual Node {
 public:
  virtual NodeType get_kind() = 0;
  virtual std::string get_expr_string() = 0;
};

class RelExpressionNodeChild : public virtual Node {
 public:
  virtual NodeType get_kind() = 0;
  virtual std::string get_expr_string() = 0;
};

class CondExpressionNodeChild : public virtual Node {
 public:
  virtual NodeType get_kind() = 0;
  virtual std::string get_expr_string() = 0;
};

class IfWhileNodeCond : public virtual Node {
 public:
  virtual NodeType get_kind() = 0;
  virtual std::string get_expr_string() = 0;
};

class IdentifierNode : public AssignNodeExpr,
                       public ExpressionNodeChild,
                       public RelExpressionNodeChild {
 public:
  IdentifierNode(std::string name, LocInfo loc);
  NodeType get_kind();
  std::string get_name();
  std::string get_expr_string();

 private:
  std::string name_;
};

class ConstantNode : public AssignNodeExpr,
                     public ExpressionNodeChild,
                     public RelExpressionNodeChild {
 public:
  ConstantNode(std::string value, LocInfo loc);
  NodeType get_kind();
  std::string get_value();
  std::string get_expr_string();

 private:
  std::string value_;
};

class ExpressionNode : public AssignNodeExpr,
                       public ExpressionNodeChild,
                       public RelExpressionNodeChild {
 public:
  ExpressionNode(ExprOp op, ExpressionNodeChild *left, ExpressionNodeChild *right, LocInfo loc);
  NodeType get_kind();
  ExprOp get_op();
  Node *get_left();
  Node *get_right();
  std::string get_expr_string();

 private:
  ExprOp op_;
  ExpressionNodeChild *left_;
  ExpressionNodeChild *right_;
};

class RelExpressionNode : public CondExpressionNodeChild, public IfWhileNodeCond {
 public:
  RelExpressionNode(RelExprOp op, RelExpressionNodeChild *left, RelExpressionNodeChild *right,
                    LocInfo loc);
  NodeType get_kind();
  RelExprOp get_op();
  Node *get_left();
  Node *get_right();
  std::string get_expr_string();

 private:
  RelExprOp op_;
  RelExpressionNodeChild *left_;
  RelExpressionNodeChild *right_;
};

class CondExpressionNode : public CondExpressionNodeChild, public IfWhileNodeCond {
 public:
  CondExpressionNode(CondExprOp op, CondExpressionNodeChild *left, CondExpressionNodeChild *right,
                     LocInfo loc);
  NodeType get_kind();
  CondExprOp get_op();
  Node *get_left();
  Node *get_right();
  std::string get_expr_string();

 private:
  CondExprOp op_;
  CondExpressionNodeChild *left_;
  CondExpressionNodeChild *right_;
};

class AssignNode : public StatementNode {
 public:
  AssignNode(IdentifierNode *var, AssignNodeExpr *expr, int stmt_no, LocInfo loc);
  NodeType get_kind();
  IdentifierNode *get_var();
  Node *expr();

 private:
  IdentifierNode *var_;
  AssignNodeExpr *expr_;
};

class IfNode : public StatementNode {
 public:
  IfNode(IfWhileNodeCond *cond, std::vector<StatementNode *> then_stmt_lst,
         std::vector<StatementNode *> else_stmt_lst, int stmt_no, LocInfo loc);
  NodeType get_kind();
  Node *get_cond();
  std::vector<StatementNode *> get_then_stmt_lst();
  std::vector<StatementNode *> get_else_stmt_lst();

 private:
  IfWhileNodeCond *cond_;
  std::vector<StatementNode *> then_stmt_lst_;
  std::vector<StatementNode *> else_stmt_lst_;
};

class WhileNode : public StatementNode {
 public:
  WhileNode(IfWhileNodeCond *cond, std::vector<StatementNode *> stmt_lst, int stmt_no, LocInfo loc);
  NodeType get_kind();
  Node *get_cond();
  std::vector<StatementNode *> get_stmt_list();

 private:
  IfWhileNodeCond *cond_;
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

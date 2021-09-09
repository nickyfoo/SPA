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

class Node {
 public:
  virtual NodeType kind() = 0;
  virtual int line_no() = 0;
  virtual int col_no() = 0;
};

class IdentifierNode : public Node {
 private:
  std::string name_;
  int line_no_;
  int col_no_;

 public:
  IdentifierNode(std::string name, int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  std::string name();
};

class ConstantNode : public Node {
 private:
  std::string value_;
  int line_no_;
  int col_no_;

 public:
  ConstantNode(std::string value, int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  std::string value();
};

class ExpressionNode : public Node {
 private:
  ExprOp op_;
  Node *left_;              // must be Expression, Constant or Identifier
  Node *right_;             // must be Expression, Constant or Identifier
  std::string expr_string_;  // original exprstring converted to postfix notation
  int line_no_;
  int col_no_;

 public:
  ExpressionNode(ExprOp op, Node *left, Node *right, std::string expr_string,
                 int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
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
  int line_no_;
  int col_no_;

 public:
  RelExpressionNode(RelExprOp op, Node *left, Node *right, int line_no,
                    int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  RelExprOp op();
  Node *left();
  Node *right();
};

class CondExpressionNode : public Node {
 private:
  CondExprOp op_;
  Node *left_;   // must be RelExpression or CondExpression
  Node *right_;  // must be RelExpression or CondExpression
  int line_no_;
  int col_no_;

 public:
  CondExpressionNode(CondExprOp op, Node *left, Node *right, int line_no,
                     int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  CondExprOp op();
  Node *left();
  Node *right();
};

class StatementNode : public Node {
 public:
  virtual int stmt_no() = 0;
};

class AssignNode : public StatementNode {
 private:
  IdentifierNode *var_;
  Node *expr_;  // must be Expression, Constant or Identifier
  int stmt_no_;
  int line_no_;
  int col_no_;

 public:
  AssignNode(IdentifierNode *var, Node *expr, int stmt_no,
             int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  int stmt_no();
  IdentifierNode *var();
  Node *expr();
};

class IfNode : public StatementNode {
 private:
  Node *cond_;  // must be RelExprNode or CondExprNode
  std::vector<StatementNode *> then_stmt_lst_;
  std::vector<StatementNode *> else_stmt_lst_;
  int stmt_no_;
  int line_no_;
  int col_no_;

 public:
  IfNode(Node *cond, std::vector<StatementNode *> then_stmt_lst,
         std::vector<StatementNode *> else_stmt_lst, int stmt_no, int line_no,
         int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  int stmt_no();
  Node *cond();
  std::vector<StatementNode *> then_stmt_lst();
  std::vector<StatementNode *> else_stmt_lst();
};

class WhileNode : public StatementNode {
 private:
  Node *cond_;  // must be RelExprNode or CondExprNode
  std::vector<StatementNode *> stmt_lst_;
  int stmt_no_;
  int line_no_;
  int col_no_;

 public:
  WhileNode(Node *cond, std::vector<StatementNode *> stmt_lst, int stmt_no,
            int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  int stmt_no();
  Node *cond();
  std::vector<StatementNode *> stmt_list();
};

class ReadNode : public StatementNode {
 private:
  IdentifierNode *var_;
  int stmt_no_;
  int line_no_;
  int col_no_;

 public:
  ReadNode(IdentifierNode *var, int stmt_no, int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  int stmt_no();
  IdentifierNode *var();
};

class PrintNode : public StatementNode {
 private:
  IdentifierNode *var_;
  int stmt_no_;
  int line_no_;
  int col_no_;

 public:
  PrintNode(IdentifierNode *var, int stmt_no, int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  int stmt_no();
  IdentifierNode *var();
};

class CallNode : public StatementNode {
 private:
  IdentifierNode *proc_;
  int stmt_no_;
  int line_no_;
  int col_no_;

 public:
  CallNode(IdentifierNode *proc, int stmt_no, int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  int stmt_no();
  IdentifierNode *proc();
};

class ProcedureNode : public Node {
 private:
  std::string name_;
  std::vector<StatementNode *> stmt_lst_;
  int line_no_;
  int col_no_;

 public:
  ProcedureNode(std::string name, std::vector<StatementNode *> stmt_lst,
                int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  std::string name();
  std::vector<StatementNode *> stmt_lst();
};

class ProgramNode : public Node {
 private:
  std::vector<ProcedureNode *> procedures_;
  int line_no_;
  int col_no_;

 public:
  ProgramNode(std::vector<ProcedureNode *> procedures, int line_no, int col_no);
  NodeType kind();
  int line_no();
  int col_no();
  std::vector<ProcedureNode *> procedures();
};

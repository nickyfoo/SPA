#pragma once

#include <string>
#include <vector>

namespace ast {
    enum Kind {
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
        Program,
        NUM_KIND
};

enum ExprOp { Plus, Minus, Times, Divide, Modulo, Noop };

enum RelExprOp { Gt, Gte, Lt, Lte, Eq, Neq };

enum CondExprOp { And, Or, Not, None };

class Node {
public:
  Kind kind;
  int lineNo;
  int colNo;

  Node() {}
  Node(Kind _kind, int _lineNo, int _colNo) : kind(_kind), lineNo(_lineNo), colNo(_colNo) {}
};

class IdentifierNode : public Node {
public:
  std::string name;

  IdentifierNode() {}
  IdentifierNode(std::string _name, int _lineNo, int _colNo)
      : Node(Kind::Identifier, _lineNo, _colNo), name(_name) {}
};

class ConstantNode : public Node {
public:
  std::string value;

  ConstantNode() {}
  ConstantNode(std::string _value, int _lineNo, int _colNo)
      : Node(Kind::Constant, _lineNo, _colNo), value(_value) {}
};

class ExpressionNode : public Node {
public:
  ExprOp op;
  Node *left;
  Node *right;

  ExpressionNode(ExprOp _op, Node *_left, Node *_right, int _lineNo, int _colNo)
      : Node(Kind::Expression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class RelExpressionNode : public Node {
public:
  RelExprOp op;
  Node *left;
  Node *right;

  RelExpressionNode(RelExprOp _op, Node *_left, Node *_right, int _lineNo, int _colNo)
      : Node(Kind::RelExpression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class CondExpressionNode : public Node {
public:
  CondExprOp op;
  Node *left;
  Node *right;

  CondExpressionNode(CondExprOp _op, Node *_left, Node *_right, int _lineNo, int _colNo)
      : Node(Kind::CondExpression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class AssignNode : public Node {
public:
  IdentifierNode *var;
  ExpressionNode *expr;

  AssignNode(IdentifierNode *_var, ExpressionNode *_expr, int _lineNo, int _colNo)
      : Node(Kind::Assign, _lineNo, _colNo), var(_var), expr(_expr) {}
};

class IfNode : public Node {
public:
  CondExpressionNode *cond;
  std::vector<Node *> thenStmtLst;
  std::vector<Node *> elseStmtLst;

  IfNode(CondExpressionNode *_cond, std::vector<Node *> _thenStmtLst,
         std::vector<Node *> _elseStmtLst, int _lineNo, int _colNo)
      : Node(Kind::If, _lineNo, _colNo), cond(_cond), thenStmtLst(_thenStmtLst),
        elseStmtLst(_elseStmtLst) {}
};

class WhileNode : public Node {
public:
  CondExpressionNode *cond;
  std::vector<Node *> stmtLst;

  WhileNode(CondExpressionNode *_cond, std::vector<Node *> _stmtLst, int _lineNo, int _colNo)
      : Node(Kind::While, _lineNo, _colNo), cond(_cond), stmtLst(_stmtLst) {}
};

class ReadNode : public Node {
public:
  IdentifierNode *var;

  ReadNode(IdentifierNode *_var, int _lineNo, int _colNo)
      : Node(Kind::Read, _lineNo, _colNo), var(_var) {}
};

class PrintNode : public Node {
public:
  IdentifierNode *var;

  PrintNode(IdentifierNode *_var, int _lineNo, int _colNo)
      : Node(Kind::Print, _lineNo, _colNo), var(_var) {}
};

class CallNode : public Node {
public:
  IdentifierNode *proc;

  CallNode(IdentifierNode *_proc, int _lineNo, int _colNo)
      : Node(Kind::Call, _lineNo, _colNo), proc(_proc) {}
};

class ProcedureNode : public Node {
public:
  std::vector<Node *> stmtLst;

  ProcedureNode(std::vector<Node *> _stmtLst, int _lineNo, int _colNo)
      : Node(Kind::Procedure, _lineNo, _colNo), stmtLst(_stmtLst) {}
};

class ProgramNode : public Node {
public:
  std::vector<ProcedureNode *> procedures;

  ProgramNode(std::vector<ProcedureNode *> _procedures, int _lineNo, int _colNo)
      : Node(Kind::Program, _lineNo, _colNo), procedures(_procedures) {}
};

std::vector<Node*> nextNodes(Node* node);
void visit(Node* node, std::vector<void (*)(Node* currentNode)> functions);

void visitWithAncestors(Node* node, std::vector<Node*>& ancestorList, std::vector<void (*) (Node* currentNode, std::vector<Node*>ancestorList)> functions);

} // namespace ast

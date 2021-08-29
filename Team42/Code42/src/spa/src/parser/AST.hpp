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
  Program
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
  Node left;
  Node right;

  ExpressionNode(){};
  ExpressionNode(ExprOp _op, Node _left, Node _right, int _lineNo, int _colNo)
      : Node(Kind::Expression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class RelExpressionNode : public Node {
public:
  RelExprOp op;
  Node left;
  Node right;

  RelExpressionNode() {}
  RelExpressionNode(RelExprOp _op, Node _left, Node _right, int _lineNo, int _colNo)
      : Node(Kind::RelExpression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class CondExpressionNode : public Node {
public:
  CondExprOp op;
  Node left;
  Node right;

  CondExpressionNode() {}
  CondExpressionNode(CondExprOp _op, Node _left, Node _right, int _lineNo, int _colNo)
      : Node(Kind::CondExpression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class AssignNode : public Node {
public:
  class IdentifierNode var;
  class ExpressionNode expr;

  AssignNode() {}
  AssignNode(class IdentifierNode _var, class ExpressionNode _expr, int _lineNo, int _colNo)
      : Node(Kind::Assign, _lineNo, _colNo), var(_var), expr(_expr) {}
};

class IfNode : public Node {
public:
  class CondExpressionNode cond;
  std::vector<Node> ifBlock;
  std::vector<Node> elseBlock;

  IfNode() {}
  IfNode(class CondExpressionNode _cond, std::vector<Node> _ifBlock, std::vector<Node> _elseBlock,
         int _lineNo, int _colNo)
      : Node(Kind::If, _lineNo, _colNo), cond(_cond), ifBlock(_ifBlock), elseBlock(_elseBlock) {}
};

class WhileNode : public Node {
public:
  class CondExpressionNode cond;
  std::vector<Node> stmtLst;

  WhileNode() {}
  WhileNode(class CondExpressionNode _cond, std::vector<Node> _stmtLst, int _lineNo, int _colNo)
      : Node(Kind::While, _lineNo, _colNo), cond(_cond), stmtLst(_stmtLst) {}
};

class ReadNode : public Node {
public:
  class IdentifierNode var;

  ReadNode() {}
  ReadNode(class IdentifierNode _var, int _lineNo, int _colNo)
      : Node(Kind::Read, _lineNo, _colNo), var(_var) {}
};

class PrintNode : public Node {
public:
  class IdentifierNode var;

  PrintNode() {}
  PrintNode(class IdentifierNode _var, int _lineNo, int _colNo)
      : Node(Kind::Print, _lineNo, _colNo), var(_var) {}
};

class CallNode : public Node {
public:
  class IdentifierNode proc;

  CallNode() {}
  CallNode(class IdentifierNode _proc, int _lineNo, int _colNo)
      : Node(Kind::Call, _lineNo, _colNo), proc(_proc) {}
};

class ProcedureNode : public Node {
public:
  std::vector<Node> stmtLst;

  ProcedureNode() {}
  ProcedureNode(std::vector<Node> _stmtLst, int _lineNo, int _colNo)
      : Node(Kind::Procedure, _lineNo, _colNo), stmtLst(_stmtLst) {}
};

class ProgramNode : public Node {
public:
  std::vector<class ProcedureNode> procedures;

  ProgramNode() {}
  ProgramNode(std::vector<class ProcedureNode> _procedures, int _lineNo, int _colNo)
      : Node(Kind::Procedure, _lineNo, _colNo), procedures(_procedures) {}
};

} // namespace ast

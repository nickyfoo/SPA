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
  Procedure,
  Program
};

enum ExprOp { Plus, Minus, Times, Divide, Modulo };

enum RelExprOp { Gt, Gte, Lt, Lte, Eq, Neq };

enum CondExprOp { And, Or, Not };

class Node {
public:
  Kind kind;
  int lineNo;
  int colNo;

  Node(Kind _kind, int _lineNo, int _colNo) : kind(_kind), lineNo(_lineNo), colNo(_colNo) {}
};

class Identifier : public Node {
public:
  std::string name;

  Identifier(std::string _name, int _lineNo, int _colNo)
      : Node(Kind::Identifier, _lineNo, _colNo), name(_name) {}
};

class Constant : public Node {
public:
  std::string value;

  Constant(std::string _value, int _lineNo, int _colNo)
      : Node(Kind::Constant, _lineNo, _colNo), value(_value) {}
};

class Expression : public Node {
public:
  ExprOp op;
  Node left;
  Node right;

  Expression(ExprOp _op, Node _left, Node _right, int _lineNo, int _colNo)
      : Node(Kind::Expression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class RelExpression : public Node {
public:
  RelExprOp op;
  Node left;
  Node right;

  RelExpression(RelExprOp _op, Node _left, Node _right, int _lineNo, int _colNo)
      : Node(Kind::RelExpression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class CondExpression : public Node {
public:
  CondExprOp op;
  Node left;
  Node right;

  CondExpression(CondExprOp _op, Node _left, Node _right, int _lineNo, int _colNo)
      : Node(Kind::CondExpression, _lineNo, _colNo), op(_op), left(_left), right(_right) {}
};

class Assign : public Node {
public:
  class Identifier var;
  class Expression expr;

  Assign(class Identifier _var, class Expression _expr, int _lineNo, int _colNo)
      : Node(Kind::Assign, _lineNo, _colNo), var(_var), expr(_expr) {}
};

class If : public Node {
public:
  class CondExpression cond;
  std::vector<Node> ifBlock;
  std::vector<Node> elseBlock;

  If(class CondExpression _cond, std::vector<Node> _ifBlock, std::vector<Node> _elseBlock,
     int _lineNo, int _colNo)
      : Node(Kind::If, _lineNo, _colNo), cond(_cond), ifBlock(_ifBlock), elseBlock(_elseBlock) {}
};

class While : public Node {
public:
  class CondExpression cond;
  std::vector<Node> stmtLst;

  While(class CondExpression _cond, std::vector<Node> _stmtLst, int _lineNo, int _colNo)
      : Node(Kind::While, _lineNo, _colNo), cond(_cond), stmtLst(_stmtLst) {}
};

class Read : public Node {
public:
  class Identifier var;

  Read(class Identifier _var, int _lineNo, int _colNo)
      : Node(Kind::Read, _lineNo, _colNo), var(_var) {}
};

class Print : public Node {
public:
  class Identifier var;

  Print(class Identifier _var, int _lineNo, int _colNo)
      : Node(Kind::Print, _lineNo, _colNo), var(_var) {}
};

class Procedure : public Node {
  std::vector<Node> stmtLst;

  Procedure(std::vector<Node> _stmtLst, int _lineNo, int _colNo)
      : Node(Kind::Procedure, _lineNo, _colNo), stmtLst(_stmtLst) {}
};

class Program : public Node {
  std::vector<class Procedure> procedures;

  Program(std::vector<class Procedure> _procedures, int _lineNo, int _colNo)
      : Node(Kind::Procedure, _lineNo, _colNo), procedures(_procedures) {}
};

} // namespace ast

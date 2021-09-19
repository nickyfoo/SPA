#include <string>
#include <vector>

#include "ast.h"
#include "catch.hpp"

class StubNode : public Node {
 private:
  NodeType kind_;

 public:
  StubNode(NodeType kind) : Node({rand(), rand()}) {
    this->kind_ = kind;
  }
  NodeType get_kind() { return this->kind_; }
};

class StubStatementNode : public StatementNode {
 private:
  NodeType kind_;

 public:
  StubStatementNode(NodeType kind)
      : StatementNode(rand(), {rand(), rand()}) {
    this->kind_ = kind;
  }

  NodeType get_kind() { return this->kind_; }
};

TEST_CASE("Test Identifier Node") {
  IdentifierNode *i;
  int ln, cn;
  std::string name;

  ln = rand();
  cn = rand();
  name = "X";
  i = new IdentifierNode(name, {ln, cn});
  REQUIRE(i->get_kind() == NodeType::Identifier);
  REQUIRE(i->get_line_no() == ln);
  REQUIRE(i->get_col_no() == cn);
  REQUIRE(i->get_name() == name);

  ln = rand();
  cn = rand();
  name = "Foo";
  i = new IdentifierNode(name, {ln, cn});
  REQUIRE(i->get_kind() == NodeType::Identifier);
  REQUIRE(i->get_line_no() == ln);
  REQUIRE(i->get_col_no() == cn);
  REQUIRE(i->get_name() == name);
}

TEST_CASE("Test Constant Node") {
  ConstantNode *c;
  int ln, cn;
  std::string value;

  ln = rand();
  cn = rand();
  value = "12345";
  c = new ConstantNode(value, {ln, cn});
  REQUIRE(c->get_kind() == NodeType::Constant);
  REQUIRE(c->get_line_no() == ln);
  REQUIRE(c->get_col_no() == cn);
  REQUIRE(c->get_value() == value);

  ln = rand();
  cn = rand();
  value = "652434141";
  c = new ConstantNode(value, {ln, cn});
  REQUIRE(c->get_kind() == NodeType::Constant);
  REQUIRE(c->get_line_no() == ln);
  REQUIRE(c->get_col_no() == cn);
  REQUIRE(c->get_value() == value);
}

TEST_CASE("Test Expression Node") {
  ExpressionNode *e;
  int ln, cn;
  ExprOp op;
  Node *left, *right;
  std::string expr_string;

  std::vector<std::tuple<ExprOp, Node *, Node *, std::string>> perms{
      {ExprOp::Plus, new StubNode(NodeType::Constant), new StubNode(NodeType::Constant), "1 2 +"},
      {ExprOp::Minus, new StubNode(NodeType::Identifier), new StubNode(NodeType::Identifier),
       "X Y -"},
      {ExprOp::Times, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant),
       "X 1 *"},
      {ExprOp::Divide, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant),
       "Y 2 /"},
      {ExprOp::Modulo, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant),
       "Y 2 %"},
  };

  for (auto p : perms) {
    ln = rand();
    cn = rand();
    op = std::get<0>(p);
    left = std::get<1>(p);
    right = std::get<2>(p);
    expr_string = std::get<3>(p);

    e = new ExpressionNode(op, left, right, expr_string, {ln, cn});

    REQUIRE(e->get_kind() == NodeType::Expression);
    REQUIRE(e->get_line_no() == ln);
    REQUIRE(e->get_col_no() == cn);
    REQUIRE(e->get_op() == op);
    REQUIRE(e->get_left() == left);
    REQUIRE(e->get_right() == right);
    REQUIRE(e->get_expr_string() == expr_string);
  }

  std::vector<std::tuple<ExprOp, Node *, Node *, std::string>> fail_perms{
      {ExprOp::Plus, nullptr, new StubNode(NodeType::Constant), "1 2 +"},
      {ExprOp::Minus, new StubNode(NodeType::Identifier), nullptr, "X Y -"},
      {ExprOp::Times, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Constant),
       "X 1 *"},
      {ExprOp::Divide, new StubNode(NodeType::Identifier), new StubNode(NodeType::CondExpression),
       "Y 2 /"},
      {ExprOp::Modulo, new StubNode(NodeType::Assign), new StubNode(NodeType::Constant), "Y 2 %"},
      {ExprOp::Plus, new StubNode(NodeType::If), new StubNode(NodeType::Constant), "1 2 +"},
      {ExprOp::Minus, new StubNode(NodeType::Identifier), new StubNode(NodeType::While), "X Y -"},
      {ExprOp::Times, new StubNode(NodeType::Read), new StubNode(NodeType::Constant), "X 1 *"},
      {ExprOp::Divide, new StubNode(NodeType::Identifier), new StubNode(NodeType::Call), "Y 2 /"},
      {ExprOp::Modulo, new StubNode(NodeType::Assign), new StubNode(NodeType::Constant), "Y 2 %"},
      {ExprOp::Divide, new StubNode(NodeType::Identifier), new StubNode(NodeType::Procedure),
       "Y 2 /"},
      {ExprOp::Modulo, new StubNode(NodeType::Program), new StubNode(NodeType::Constant), "Y 2 %"},
  };

  for (auto p : fail_perms) {
    ln = rand();
    cn = rand();
    op = std::get<0>(p);
    left = std::get<1>(p);
    right = std::get<2>(p);
    expr_string = std::get<3>(p);
    REQUIRE_THROWS_AS(
        ExpressionNode(op, left, right, expr_string, {ln, cn}),
        std::invalid_argument);
  }
}

TEST_CASE("Test RelExpression Node") {
  RelExpressionNode *r;
  int ln, cn;
  RelExprOp op;
  Node *left, *right;
  std::string expr_string;

  std::vector<std::tuple<RelExprOp, Node *, Node *, std::string>> perms{
      {RelExprOp::Gt, new StubNode(NodeType::Constant), new StubNode(NodeType::Constant), "1 2 >"},
      {RelExprOp::Gte, new StubNode(NodeType::Identifier), new StubNode(NodeType::Identifier), "x y >="},
      {RelExprOp::Lt, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant), "a 1 <"},
      {RelExprOp::Lte, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant), "Y 3 <="},
      {RelExprOp::Eq, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant), "Z 50 =="},
      {RelExprOp::Neq, new StubNode(NodeType::Identifier), new StubNode(NodeType::Constant), "foo 5 !="},
  };

  for (auto p : perms) {
    ln = rand();
    cn = rand();
    op = std::get<0>(p);
    left = std::get<1>(p);
    right = std::get<2>(p);
    expr_string = std::get<3>(p);

    r = new RelExpressionNode(op, left, right, expr_string, {ln, cn});

    REQUIRE(r->get_kind() == NodeType::RelExpression);
    REQUIRE(r->get_line_no() == ln);
    REQUIRE(r->get_col_no() == cn);
    REQUIRE(r->get_op() == op);
    REQUIRE(r->get_left() == left);
    REQUIRE(r->get_right() == right);
    REQUIRE(r->get_expr_string() == expr_string);
  }

  std::vector<std::tuple<RelExprOp, Node *, Node *>> fail_perms{
      {RelExprOp::Lte, nullptr, new StubNode(NodeType::Constant)},
      {RelExprOp::Gt, new StubNode(NodeType::Identifier), nullptr},
      {RelExprOp::Eq, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Constant)},
      {RelExprOp::Neq, new StubNode(NodeType::Identifier), new StubNode(NodeType::CondExpression)},
      {RelExprOp::Gte, new StubNode(NodeType::Assign), new StubNode(NodeType::Constant)},
      {RelExprOp::Gt, new StubNode(NodeType::If), new StubNode(NodeType::Constant)},
      {RelExprOp::Lte, new StubNode(NodeType::Identifier), new StubNode(NodeType::While)},
      {RelExprOp::Eq, new StubNode(NodeType::Read), new StubNode(NodeType::Constant)},
      {RelExprOp::Lt, new StubNode(NodeType::Identifier), new StubNode(NodeType::Call)},
      {RelExprOp::Neq, new StubNode(NodeType::Assign), new StubNode(NodeType::Constant)},
      {RelExprOp::Gt, new StubNode(NodeType::Identifier), new StubNode(NodeType::Procedure)},
      {RelExprOp::Gte, new StubNode(NodeType::Program), new StubNode(NodeType::Constant)},
  };

  for (auto p : fail_perms) {
    ln = rand();
    cn = rand();
    op = std::get<0>(p);
    left = std::get<1>(p);
    right = std::get<2>(p);
    REQUIRE_THROWS_AS(RelExpressionNode(op, left, right, "", {ln, cn}),
                      std::invalid_argument);
  }
}

TEST_CASE("Test CondExpression Node") {
  CondExpressionNode *c;
  int ln, cn;
  CondExprOp op;
  Node *left, *right;
  std::string expr_string;

  std::vector<std::tuple<CondExprOp, Node *, Node *, std::string>> perms{
      {CondExprOp::And, new StubNode(NodeType::CondExpression),
       new StubNode(NodeType::CondExpression), "x y > ! a b < ! &&"},
      {CondExprOp::Or, new StubNode(NodeType::RelExpression), new StubNode(NodeType::RelExpression),
       "x y > a b == ||"},
      {CondExprOp::And, new StubNode(NodeType::RelExpression),
       new StubNode(NodeType::CondExpression), "x y != a b == ! &&"},
      {CondExprOp::Or, new StubNode(NodeType::CondExpression),
       new StubNode(NodeType::RelExpression), "x y != a b == && i j > ||"},
      {CondExprOp::Not, new StubNode(NodeType::CondExpression), nullptr, "x y >= ! !"},
      {CondExprOp::Not, new StubNode(NodeType::RelExpression), nullptr, "a == b !"}};

  for (auto p : perms) {
    ln = rand();
    cn = rand();
    op = std::get<0>(p);
    left = std::get<1>(p);
    right = std::get<2>(p);
    expr_string = std::get<3>(p);

    c = new CondExpressionNode(op, left, right, expr_string, {ln, cn});

    REQUIRE(c->get_kind() == NodeType::CondExpression);
    REQUIRE(c->get_line_no() == ln);
    REQUIRE(c->get_col_no() == cn);
    REQUIRE(c->get_op() == op);
    REQUIRE(c->get_left() == left);
    REQUIRE(c->get_right() == right);
    REQUIRE(c->get_expr_string() == expr_string);
  }

  std::vector<std::tuple<CondExprOp, Node *, Node *>> fail_perms{
      {CondExprOp::Not, nullptr, new StubNode(NodeType::CondExpression)},
      {CondExprOp::Not, new StubNode(NodeType::RelExpression),
       new StubNode(NodeType::RelExpression)},
      {CondExprOp::Not, new StubNode(NodeType::Constant), nullptr},
      {CondExprOp::Not, new StubNode(NodeType::While), nullptr},
      {CondExprOp::Not, new StubNode(NodeType::Expression), nullptr},

      {CondExprOp::And, nullptr, new StubNode(NodeType::CondExpression)},
      {CondExprOp::Or, nullptr, new StubNode(NodeType::RelExpression)},
      {CondExprOp::And, new StubNode(NodeType::CondExpression), nullptr},
      {CondExprOp::Or, new StubNode(NodeType::RelExpression), nullptr},

      {CondExprOp::And, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Constant)},
      {CondExprOp::Or, new StubNode(NodeType::Identifier), new StubNode(NodeType::CondExpression)},
      {CondExprOp::And, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Expression)},
      {CondExprOp::Or, new StubNode(NodeType::Identifier), new StubNode(NodeType::CondExpression)},
      {CondExprOp::And, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Constant)},
      {CondExprOp::Or, new StubNode(NodeType::Assign), new StubNode(NodeType::CondExpression)},
      {CondExprOp::And, new StubNode(NodeType::RelExpression), new StubNode(NodeType::If)},
      {CondExprOp::Or, new StubNode(NodeType::While), new StubNode(NodeType::CondExpression)},
      {CondExprOp::And, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Read)},
      {CondExprOp::Or, new StubNode(NodeType::Print), new StubNode(NodeType::CondExpression)},
      {CondExprOp::And, new StubNode(NodeType::RelExpression), new StubNode(NodeType::Call)},
      {CondExprOp::Or, new StubNode(NodeType::Procedure), new StubNode(NodeType::CondExpression)},
      {CondExprOp::Or, new StubNode(NodeType::Program), new StubNode(NodeType::CondExpression)},
  };

  for (auto p : fail_perms) {
    ln = rand();
    cn = rand();
    op = std::get<0>(p);
    left = std::get<1>(p);
    right = std::get<2>(p);
    REQUIRE_THROWS_AS(CondExpressionNode(op, left, right, "", {ln, cn}),
                      std::invalid_argument);
  }
}

TEST_CASE("Test Assign Node") {
  AssignNode *a;
  IdentifierNode *i;
  Node *n;
  int ln, cn, sn;

  ln = rand();
  cn = rand();
  sn = rand();
  i = new IdentifierNode("X", {ln, cn});
  n = new StubNode(NodeType::Expression);

  a = new AssignNode(i, n, sn, {ln, cn});
  REQUIRE(a->get_kind() == NodeType::Assign);
  REQUIRE(a->get_line_no() == ln);
  REQUIRE(a->get_col_no() == cn);
  REQUIRE(a->get_var() == i);
  REQUIRE(a->expr() == n);
  REQUIRE(a->get_stmt_no() == sn);

  ln = rand();
  cn = rand();
  sn = rand();
  i = new IdentifierNode("X", {ln, cn});
  n = new StubNode(NodeType::Constant);

  a = new AssignNode(i, n, sn, {ln, cn});
  REQUIRE(a->get_kind() == NodeType::Assign);
  REQUIRE(a->get_line_no() == ln);
  REQUIRE(a->get_col_no() == cn);
  REQUIRE(a->get_var() == i);
  REQUIRE(a->expr() == n);
  REQUIRE(a->get_stmt_no() == sn);

  ln = rand();
  cn = rand();
  sn = rand();
  i = new IdentifierNode("X", {ln, cn});
  n = new StubNode(NodeType::Constant);

  a = new AssignNode(i, n, sn, {ln, cn});
  REQUIRE(a->get_kind() == NodeType::Assign);
  REQUIRE(a->get_line_no() == ln);
  REQUIRE(a->get_col_no() == cn);
  REQUIRE(a->get_var() == i);
  REQUIRE(a->expr() == n);
  REQUIRE(a->get_stmt_no() == sn);

  std::vector<NodeType> fail_perms{
      NodeType::RelExpression, NodeType::CondExpression, NodeType::Assign, NodeType::If,
      NodeType::While,         NodeType::Read,           NodeType::Print,  NodeType::Call,
      NodeType::Procedure,     NodeType::Program};

  for (auto p : fail_perms) {
    ln = rand();
    cn = rand();
    i = new IdentifierNode("X", {ln, cn});
    n = new StubNode(p);
    REQUIRE_THROWS_AS(AssignNode(i, n, 3, {ln, cn}),
                      std::invalid_argument);
  }

  ln = rand();
  cn = rand();
  i = nullptr;
  n = new StubNode(NodeType::Identifier);
  REQUIRE_THROWS_AS(AssignNode(i, n, 3, {ln, cn}),
                    std::invalid_argument);

  ln = rand();
  cn = rand();
  i = new IdentifierNode("X", {ln, cn});
  n = nullptr;
  REQUIRE_THROWS_AS(AssignNode(i, n, 3, {ln, cn}),
                    std::invalid_argument);
}

TEST_CASE("Test If Node") {
  IfNode *i;
  Node *cond;
  std::vector<StatementNode *> then_lst;
  std::vector<StatementNode *> else_lst;
  int ln, cn, sn;

  std::function<std::vector<StatementNode *>()> populateStmtLst = []() {
    std::vector<NodeType> stmtTypes = {NodeType::Assign, NodeType::If,    NodeType::While,
                                       NodeType::Read,   NodeType::Print, NodeType::Call};

    std::vector<StatementNode *> v;
    int count = rand() % 10;
    for (int j = 0; j <= count; j++) {
      v.push_back(new StubStatementNode(stmtTypes[rand() % 6]));
    }

    return v;
  };

  cond = new StubNode(NodeType::RelExpression);
  then_lst = populateStmtLst();
  else_lst = populateStmtLst();
  ln = rand();
  cn = rand();
  sn = rand();

  i = new IfNode(cond, then_lst, else_lst, sn, {ln, cn});
  REQUIRE(i->get_kind() == NodeType::If);
  REQUIRE(i->get_line_no() == ln);
  REQUIRE(i->get_col_no() == cn);
  REQUIRE(i->get_stmt_no() == sn);
  REQUIRE(i->get_cond() == cond);
  REQUIRE(i->get_else_stmt_lst() == else_lst);
  REQUIRE(i->get_then_stmt_lst() == then_lst);

  cond = new StubNode(NodeType::CondExpression);
  then_lst = populateStmtLst();
  else_lst = populateStmtLst();
  ln = rand();
  cn = rand();
  sn = rand();

  i = new IfNode(cond, then_lst, else_lst, sn, {ln, cn});
  REQUIRE(i->get_kind() == NodeType::If);
  REQUIRE(i->get_line_no() == ln);
  REQUIRE(i->get_col_no() == cn);
  REQUIRE(i->get_stmt_no() == sn);
  REQUIRE(i->get_cond() == cond);
  REQUIRE(i->get_else_stmt_lst() == else_lst);
  REQUIRE(i->get_then_stmt_lst() == then_lst);

  cond = nullptr;
  then_lst = populateStmtLst();
  else_lst = populateStmtLst();
  ln = rand();
  cn = rand();
  sn = rand();
  REQUIRE_THROWS_AS(IfNode(cond, then_lst, else_lst, sn, {ln, cn}),
                    std::invalid_argument);

  std::vector<NodeType> fail_perms{NodeType::Identifier, NodeType::Constant, NodeType::Expression,
                                   NodeType::Assign,     NodeType::If,       NodeType::While,
                                   NodeType::Read,       NodeType::Print,    NodeType::Call,
                                   NodeType::Procedure,  NodeType::Program};

  for (auto p : fail_perms) {
    cond = new StubNode(p);
    then_lst = populateStmtLst();
    else_lst = populateStmtLst();
    ln = rand();
    cn = rand();
    sn = rand();
    REQUIRE_THROWS_AS(IfNode(cond, then_lst, else_lst, sn, {ln, cn}),
                      std::invalid_argument);
  }
}

TEST_CASE("Test While Node") {
  WhileNode *w;
  Node *cond;
  std::vector<StatementNode *> stmt_lst;
  int ln, cn, sn;

  std::function<std::vector<StatementNode *>()> populateStmtLst = []() {
    std::vector<NodeType> stmtTypes = {NodeType::Assign, NodeType::If,    NodeType::While,
                                       NodeType::Read,   NodeType::Print, NodeType::Call};

    std::vector<StatementNode *> v;
    int count = rand() % 10;
    for (int j = 0; j <= count; j++) {
      v.push_back(new StubStatementNode(stmtTypes[rand() % 6]));
    }

    return v;
  };

  cond = new StubNode(NodeType::RelExpression);
  stmt_lst = populateStmtLst();
  ln = rand();
  cn = rand();
  sn = rand();

  w = new WhileNode(cond, stmt_lst, sn, {ln, cn});
  REQUIRE(w->get_kind() == NodeType::While);
  REQUIRE(w->get_line_no() == ln);
  REQUIRE(w->get_col_no() == cn);
  REQUIRE(w->get_stmt_no() == sn);
  REQUIRE(w->get_cond() == cond);
  REQUIRE(w->get_stmt_list() == stmt_lst);

  cond = new StubNode(NodeType::CondExpression);
  stmt_lst = populateStmtLst();
  ln = rand();
  cn = rand();
  sn = rand();

  w = new WhileNode(cond, stmt_lst, sn, {ln, cn});
  REQUIRE(w->get_kind() == NodeType::While);
  REQUIRE(w->get_line_no() == ln);
  REQUIRE(w->get_col_no() == cn);
  REQUIRE(w->get_stmt_no() == sn);
  REQUIRE(w->get_cond() == cond);
  REQUIRE(w->get_stmt_list() == stmt_lst);

  cond = nullptr;
  stmt_lst = populateStmtLst();
  ln = rand();
  cn = rand();
  sn = rand();
  REQUIRE_THROWS_AS(WhileNode(cond, stmt_lst, sn, {ln, cn}),
                    std::invalid_argument);

  std::vector<NodeType> fail_perms{NodeType::Identifier, NodeType::Constant, NodeType::Expression,
                                   NodeType::Assign,     NodeType::If,       NodeType::While,
                                   NodeType::Read,       NodeType::Print,    NodeType::Call,
                                   NodeType::Procedure,  NodeType::Program};

  for (auto p : fail_perms) {
    cond = new StubNode(p);
    stmt_lst = populateStmtLst();
    ln = rand();
    cn = rand();
    sn = rand();
    REQUIRE_THROWS_AS(WhileNode(cond, stmt_lst, sn, {ln, cn}),
                      std::invalid_argument);
  }
}

TEST_CASE("Test Read Node") {
  ReadNode *r;
  IdentifierNode *i;
  int ln, cn, sn;

  ln = rand();
  cn = rand();
  sn = rand();
  i = new IdentifierNode("X", {ln, cn});

  r = new ReadNode(i, sn, {ln, cn});
  REQUIRE(r->get_kind() == NodeType::Read);
  REQUIRE(r->get_line_no() == ln);
  REQUIRE(r->get_col_no() == cn);
  REQUIRE(r->get_stmt_no() == sn);
  REQUIRE(r->get_var() == i);
}

TEST_CASE("Test Print Node") {
  PrintNode *p;
  IdentifierNode *i;
  int ln, cn, sn;

  ln = rand();
  cn = rand();
  sn = rand();
  i = new IdentifierNode("X", {ln, cn});

  p = new PrintNode(i, sn, {ln, cn});
  REQUIRE(p->get_kind() == NodeType::Print);
  REQUIRE(p->get_line_no() == ln);
  REQUIRE(p->get_col_no() == cn);
  REQUIRE(p->get_stmt_no() == sn);
  REQUIRE(p->get_var() == i);
}

TEST_CASE("Test Call Node") {
  CallNode *c;
  IdentifierNode *i;
  int ln, cn, sn;

  ln = rand();
  cn = rand();
  sn = rand();
  i = new IdentifierNode("X", {ln, cn});

  c = new CallNode(i, sn, {ln, cn});
  REQUIRE(c->get_kind() == NodeType::Call);
  REQUIRE(c->get_line_no() == ln);
  REQUIRE(c->get_col_no() == cn);
  REQUIRE(c->get_stmt_no() == sn);
  REQUIRE(c->get_proc() == i);
}

TEST_CASE("Test Procedure Node") {
  ProcedureNode *p;
  std::string name;
  std::vector<StatementNode *> stmt_lst;
  int ln, cn;

  std::function<std::vector<StatementNode *>()> populateStmtLst = []() {
    std::vector<NodeType> stmtTypes = {NodeType::Assign, NodeType::If,    NodeType::While,
                                       NodeType::Read,   NodeType::Print, NodeType::Call};

    std::vector<StatementNode *> v;
    int count = rand() % 10;
    for (int j = 0; j <= count; j++) {
      v.push_back(new StubStatementNode(stmtTypes[rand() % 6]));
    }

    return v;
  };

  name = "ComputeCentroid";
  stmt_lst = populateStmtLst();
  ln = rand();
  cn = rand();

  p = new ProcedureNode(name, stmt_lst, {ln, cn});
  REQUIRE(p->get_kind() == NodeType::Procedure);
  REQUIRE(p->get_name() == name);
  REQUIRE(p->get_line_no() == ln);
  REQUIRE(p->get_col_no() == cn);

  name = "Main";
  stmt_lst = populateStmtLst();
  ln = rand();
  cn = rand();

  p = new ProcedureNode(name, stmt_lst, {ln, cn});
  REQUIRE(p->get_kind() == NodeType::Procedure);
  REQUIRE(p->get_name() == name);
  REQUIRE(p->get_line_no() == ln);
  REQUIRE(p->get_col_no() == cn);
}

TEST_CASE("Test Program Node") {
  ProgramNode *p;
  std::vector<ProcedureNode *> procedures;
  int ln, cn;

  ln = rand();
  cn = rand();
  p = new ProgramNode(procedures, {ln, cn});
  REQUIRE(p->get_kind() == NodeType::Program);
  REQUIRE(p->get_procedures() == procedures);
  REQUIRE(p->get_line_no() == ln);
  REQUIRE(p->get_col_no() == cn);
}

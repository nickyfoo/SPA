#pragma once

#include <string>
#include <vector>

#include "pkb.h"

class StatementTableStub : public StmtTable {
 protected:
  std::map<int, Statement> table_;

 public:
  void AddStmt(Statement statement) {
    table_[statement.get_stmt_no()] = statement;
  }

  std::vector<Statement *> get_all_stmts() {
    std::vector<Statement *> ans;
    for (auto &[stmt_no, stmt] : table_) {
      ans.push_back(&stmt);
    }
    return ans;
  }

  Statement *get_stmt(int line_no) {
    if (table_.find(line_no) == table_.end()) {
      return nullptr;
    }
    return &table_[line_no];
  }
};

class ProcedureTableStub : public ProcTable {
 protected:
  std::vector<Procedure> table_;
  std::map<std::string, int> name_to_index_;

 public:
  void AddProc(Procedure proc) {
    int pos = table_.size();
    table_.push_back(proc);
    name_to_index_[proc.get_name()] = pos;
  }

  std::vector<Procedure *> get_all_procs() {
    std::vector<Procedure *> ans;
    for (auto &proc : table_) {
      ans.push_back(&proc);
    }
    return ans;
  }

  Procedure *get_proc(const std::string &name) {
    if (name_to_index_.find(name) == name_to_index_.end()) {
      return nullptr;
    }
    return &table_[name_to_index_[name]];
  }
};

class VarTableStub : public VarTable {
 protected:
  std::vector<Variable> table_;

 public:
  void AddVar(Variable variable) {
    table_.push_back(variable);
  }

  std::vector<Variable *> get_all_vars() {
    std::vector<Variable *> ans;
    for (auto &var : table_) {
      ans.push_back(&var);
    }
    return ans;
  }
};

class PKBStub : public PKB {
 protected:
  ProcedureTableStub proc_table_;
  StatementTableStub stmt_table_;
  VarTableStub var_table_;
  ConstTable const_table_;
 public:

  void set_stmt_table(StatementTableStub stmt_table) {
    stmt_table_ = stmt_table;
  }
  void set_proc_table(ProcedureTableStub proc_table) {
    proc_table_ = proc_table;
  }
  void set_var_table(VarTableStub var_table) {
    var_table_ = var_table;
  }
  void set_const_table(ConstTable const_table) {
    const_table_ = const_table;
  }

  std::vector<Statement *> get_all_stmts() {
    return stmt_table_.get_all_stmts();
  }

  Statement *get_statement(int line_no) override {
    return stmt_table_.get_stmt(line_no);
  }

  std::vector<Variable *> get_all_vars() {
    return var_table_.get_all_vars();
  }

  std::vector<Procedure *> get_all_procs() {
    return proc_table_.get_all_procs();
  }

  Procedure *get_procedure(std::string &name) override {
    return proc_table_.get_proc(name);
  }

  static PKBStub *BuildPKB() {
    PKBStub *pkb_stub = new PKBStub();
    StatementTableStub stmt_table = StatementTableStub();
    ProcedureTableStub proc_table = ProcedureTableStub();
    VarTableStub var_table = VarTableStub();
    ConstTable const_table = ConstTable();

    // Creating Statements and adding to stmt table
    Statement stmt1 = Statement(1, NodeType::Assign);
    stmt1.AddFollower(2);
    stmt1.AddFollowerStar(2);
    stmt1.AddFollowerStar(3);
    stmt1.AddFollowerStar(4);
    stmt1.AddModifies("x");
    stmt_table.AddStmt(stmt1);

    Statement stmt2 = Statement(2, NodeType::Assign);
    stmt2.AddFollower(3);
    stmt2.AddFollowerStar(3);
    stmt2.AddFollowerStar(4);
    stmt2.AddFollowee(1);
    stmt2.AddFolloweeStar(1);
    stmt2.AddModifies("z");
    stmt_table.AddStmt(stmt2);

    Statement stmt3 = Statement(3, NodeType::Assign);
    stmt3.AddFollower(4);
    stmt3.AddFollowerStar(4);
    stmt3.AddFollowee(2);
    stmt3.AddFolloweeStar(1);
    stmt3.AddFolloweeStar(2);
    stmt3.AddModifies("i");
    stmt_table.AddStmt(stmt3);

    Statement stmt4 = Statement(4, NodeType::While);
    stmt4.AddFollowee(3);
    stmt4.AddFolloweeStar(1);
    stmt4.AddFolloweeStar(2);
    stmt4.AddFolloweeStar(3);
    stmt4.AddChild(5);
    stmt4.AddChild(6);
    stmt4.AddChild(9);
    stmt4.AddChild(10);
    stmt4.AddChild(11);
    stmt4.AddChildStar(5);
    stmt4.AddChildStar(6);
    stmt4.AddChildStar(7);
    stmt4.AddChildStar(8);
    stmt4.AddChildStar(9);
    stmt4.AddChildStar(10);
    stmt4.AddChildStar(11);
    stmt4.AddModifies("i");
    stmt4.AddModifies("x");
    stmt4.AddModifies("y");
    stmt4.AddModifies("z");
    stmt4.AddUses("i");
    stmt4.AddUses("x");
    stmt4.AddUses("z");
    stmt_table.AddStmt(stmt4);

    Statement stmt5 = Statement(5, NodeType::Assign);
    stmt5.AddFollower(6);
    stmt5.AddFollowerStar(6);
    stmt5.AddFollowerStar(9);
    stmt5.AddFollowerStar(10);
    stmt5.AddFollowerStar(11);
    stmt5.AddParent(4);
    stmt5.AddParentStar(4);
    stmt5.AddModifies("x");
    stmt5.AddUses("x");
    stmt_table.AddStmt(stmt5);

    Statement stmt6 = Statement(6, NodeType::If);
    stmt6.AddFollower(9);
    stmt6.AddFollowerStar(9);
    stmt6.AddFollowerStar(10);
    stmt6.AddFollowerStar(11);
    stmt6.AddFollowee(5);
    stmt6.AddFolloweeStar(5);
    stmt6.AddParent(4);
    stmt6.AddParentStar(4);
    stmt6.AddChild(7);
    stmt6.AddChild(8);
    stmt6.AddChildStar(7);
    stmt6.AddChildStar(8);
    stmt6.AddModifies("y");
    stmt6.AddModifies("z");
    stmt6.AddUses("x");
    stmt6.AddUses("z");
    stmt_table.AddStmt(stmt6);

    Statement stmt7 = Statement(7, NodeType::Assign);
    stmt7.AddParent(6);
    stmt7.AddParentStar(4);
    stmt7.AddParentStar(6);
    stmt7.AddModifies("z");
    stmt7.AddUses("x");
    stmt_table.AddStmt(stmt7);

    Statement stmt8 = Statement(8, NodeType::Assign);
    stmt8.AddParent(6);
    stmt8.AddParentStar(4);
    stmt8.AddParentStar(6);
    stmt8.AddModifies("y");
    stmt8.AddUses("x");
    stmt8.AddUses("z");
    stmt_table.AddStmt(stmt8);

    Statement stmt9 = Statement(9, NodeType::Assign);
    stmt9.AddFollower(10);
    stmt9.AddFollowerStar(10);
    stmt9.AddFollowerStar(11);
    stmt9.AddFollowee(6);
    stmt9.AddFolloweeStar(5);
    stmt9.AddFolloweeStar(6);
    stmt9.AddParent(4);
    stmt9.AddParentStar(4);
    stmt9.AddModifies("z");
    stmt9.AddUses("i");
    stmt9.AddUses("x");
    stmt9.AddUses("z");
    stmt_table.AddStmt(stmt9);

    Statement stmt10 = Statement(10, NodeType::Call);
    stmt10.AddFollower(11);
    stmt10.AddFollowerStar(11);
    stmt10.AddFollowee(9);
    stmt10.AddFolloweeStar(5);
    stmt10.AddFolloweeStar(6);
    stmt10.AddFolloweeStar(9);
    stmt10.AddParent(4);
    stmt10.AddParentStar(4);
    stmt10.AddModifies("x");
    stmt10.AddModifies("z");
    stmt10.AddUses("x");
    stmt10.AddUses("z");
    stmt_table.AddStmt(stmt10);

    Statement stmt11 = Statement(11, NodeType::Assign);
    stmt11.AddFollowee(10);
    stmt11.AddFolloweeStar(5);
    stmt11.AddFolloweeStar(6);
    stmt11.AddFolloweeStar(9);
    stmt11.AddFolloweeStar(10);
    stmt11.AddParent(4);
    stmt11.AddParentStar(4);
    stmt11.AddModifies("i");
    stmt11.AddUses("i");
    stmt_table.AddStmt(stmt11);

    Statement stmt12 = Statement(12, NodeType::If);
    stmt12.AddChild(13);
    stmt12.AddChild(14);
    stmt12.AddChildStar(13);
    stmt12.AddChildStar(14);
    stmt12.AddModifies("x");
    stmt12.AddModifies("z");
    stmt12.AddUses("x");
    stmt12.AddUses("z");
    stmt_table.AddStmt(stmt12);

    Statement stmt13 = Statement(13, NodeType::Assign);
    stmt13.AddParent(12);
    stmt13.AddParentStar(12);
    stmt13.AddModifies("z");
    stmt13.AddUses("x");
    stmt_table.AddStmt(stmt13);

    Statement stmt14 = Statement(14, NodeType::Assign);
    stmt14.AddParent(12);
    stmt14.AddParentStar(12);
    stmt14.AddModifies("x");
    stmt14.AddUses("x");
    stmt14.AddUses("z");
    stmt_table.AddStmt(stmt14);

    // Creating procedures and adding to proc table.
    Procedure example = Procedure("Example");
    example.AddUses("i");
    example.AddUses("x");
    example.AddUses("z");
    example.AddModifies("i");
    example.AddModifies("x");
    example.AddModifies("y");
    example.AddModifies("z");
    example.AddCalls("q");
    example.AddCallsStar("q");
    proc_table.AddProc(example);

    Procedure q = Procedure("q");
    q.AddUses("x");
    q.AddUses("z");
    q.AddModifies("x");
    q.AddModifies("z");
    q.AddCallers("Example");
    q.AddCallersStar("Example");
    proc_table.AddProc(q);

    // Adding variables to var table
    Variable x = Variable(0, "x");
    x.AddStmtModifying(1);
    x.AddStmtModifying(4);
    x.AddStmtModifying(5);
    x.AddStmtModifying(10);
    x.AddStmtModifying(12);
    x.AddStmtModifying(14);
    x.AddProcModifying("Example");
    x.AddProcModifying("q");
    x.AddStmtUsing(4);
    x.AddStmtUsing(5);
    x.AddStmtUsing(6);
    x.AddStmtUsing(7);
    x.AddStmtUsing(8);
    x.AddStmtUsing(9);
    x.AddStmtUsing(10);
    x.AddStmtUsing(12);
    x.AddStmtUsing(13);
    x.AddStmtUsing(14);
    x.AddProcUsing("Example");
    x.AddProcUsing("q");
    var_table.AddVar(x);

    Variable z = Variable(1, "z");
    z.AddStmtModifying(2);
    z.AddStmtModifying(4);
    z.AddStmtModifying(6);
    z.AddStmtModifying(7);
    z.AddStmtModifying(9);
    z.AddStmtModifying(10);
    z.AddStmtModifying(12);
    z.AddStmtModifying(13);
    z.AddProcModifying("Example");
    z.AddProcModifying("q");
    z.AddStmtUsing(4);
    z.AddStmtUsing(6);
    z.AddStmtUsing(8);
    z.AddStmtUsing(9);
    z.AddStmtUsing(10);
    z.AddStmtUsing(12);
    z.AddStmtUsing(14);
    z.AddProcUsing("Example");
    z.AddProcUsing("q");
    var_table.AddVar(z);

    Variable i = Variable(2, "i");
    i.AddStmtModifying(3);
    i.AddStmtModifying(4);
    i.AddStmtModifying(11);
    i.AddProcModifying("Example");
    i.AddStmtUsing(4);
    i.AddStmtUsing(9);
    i.AddStmtUsing(11);
    i.AddProcUsing("Example");
    var_table.AddVar(i);

    Variable y = Variable(3, "y");
    y.AddStmtModifying(4);
    y.AddStmtModifying(6);
    y.AddStmtModifying(8);
    y.AddProcModifying("Example");
    var_table.AddVar(y);

    // Adding constants to const table.
    const_table.AddConstant("0");
    const_table.AddConstant("1");
    const_table.AddConstant("2");
    const_table.AddConstant("3");
    const_table.AddConstant("5");

    // Setting tables in pkb
    pkb_stub->set_const_table(const_table);
    pkb_stub->set_proc_table(proc_table);
    pkb_stub->set_stmt_table(stmt_table);
    pkb_stub->set_var_table(var_table);

    return pkb_stub;
  }
};

#include <vector>

#include "catch.hpp"
#include "stmt_table.h"

class StubNode : public Node {
 private:
  NodeType kind_;

 public:
  StubNode(NodeType kind) : Node({rand(), rand()}) { this->kind_ = kind; }
  NodeType get_kind() { return this->kind_; }
};

class StubStatementNode : public StatementNode {
 private:
  NodeType kind_;

 public:
  StubStatementNode(NodeType kind) : StatementNode(rand(), {rand(), rand()}) { this->kind_ = kind; }

  NodeType get_kind() { return this->kind_; }
  std::string ToString() { return ""; }
};

TEST_CASE("GetNumStatements_DifferentSizeTables_ReturnsNumStatements", "[statement][table]") {
  SECTION("Table size: 1") {
    StmtTable stmt_table;
    // TODO(huiting): Find a way to remove the new instantiation
    stmt_table.AddStatement(new StubStatementNode(NodeType::Assign));

    REQUIRE(stmt_table.get_num_statements() == 1);
  }

  SECTION("Table size: 6, All statements") {
    StmtTable stmt_table;
    std::vector<NodeType> stmt_kinds = {NodeType::Assign, NodeType::If, NodeType::While,
                                        NodeType::Read, NodeType::Print};

    for (auto &stmt_kind : stmt_kinds) {
      stmt_table.AddStatement(new StubStatementNode(stmt_kind));
    }

    REQUIRE(stmt_table.get_num_statements() == stmt_kinds.size());
  }
}

TEST_CASE("AddGetAllStatements_DifferentSizeTables_ReturnsVectorStatements", "[statement][table]") {
  SECTION("Table size: 1") {
    StmtTable stmt_table;
    stmt_table.AddStatement(new StubStatementNode(NodeType::Assign));
    std::vector<Statement *> stmts_vec = stmt_table.get_all_statements();

    REQUIRE(stmts_vec.size() == 1);
    REQUIRE(std::any_of(stmts_vec.begin(), stmts_vec.end(),
                        [&](Statement *elem) { return elem->get_kind() == NodeType::Assign; }));
  }

  SECTION("Table size: 5") {
    StmtTable stmt_table;
    std::vector<NodeType> stmt_kinds = {NodeType::Assign, NodeType::If, NodeType::While,
                                        NodeType::Read, NodeType::Print};

    for (auto &stmt_kind : stmt_kinds) {
      stmt_table.AddStatement(new StubStatementNode(stmt_kind));
    }
    std::vector<Statement *> stmts_vec = stmt_table.get_all_statements();

    REQUIRE(stmts_vec.size() == stmt_kinds.size());
    for (auto &stmt_kind : stmt_kinds) {
      REQUIRE(std::any_of(stmts_vec.begin(), stmts_vec.end(),
                          [&](Statement *elem) { return elem->get_kind() == stmt_kind; }));
    }
  }
}

TEST_CASE("GetStatements_DifferentSizeTables_ReturnsVectorStatements", "[statement][table]") {
  SECTION("Table size: 1") {
    StmtTable stmt_table;
    stmt_table.AddStatement(new StubStatementNode(NodeType::Assign));
    stmt_table.CategorizeStatements();
    std::vector<Statement *> assign_stmts_vec = stmt_table.get_statements(NodeType::Assign);

    REQUIRE(assign_stmts_vec.size() == 1);
    REQUIRE(std::any_of(assign_stmts_vec.begin(), assign_stmts_vec.end(),
                        [&](Statement *elem) { return elem->get_kind() == NodeType::Assign; }));

    std::vector<NodeType> incorrect_stmt_kinds = {NodeType::If, NodeType::While, NodeType::Read,
                                                  NodeType::Print};
    for (auto &stmt_kind : incorrect_stmt_kinds) {
      std::vector<Statement *> incorrect_stmts_vec = stmt_table.get_statements(stmt_kind);
      REQUIRE(incorrect_stmts_vec.empty());
    }
  }

  SECTION("Table size: 5") {
    StmtTable stmt_table;
    std::vector<NodeType> stmt_kinds = {NodeType::Assign, NodeType::If, NodeType::While,
                                        NodeType::Read, NodeType::Print};

    for (auto &stmt_kind : stmt_kinds) {
      stmt_table.AddStatement(new StubStatementNode(stmt_kind));
    }
    stmt_table.CategorizeStatements();

    for (auto &stmt_kind : stmt_kinds) {
      std::vector<Statement *> stmts_vec = stmt_table.get_statements(stmt_kind);
      REQUIRE(stmts_vec.size() == 1);
      REQUIRE(std::any_of(stmts_vec.begin(), stmts_vec.end(),
                          [&](Statement *elem) { return elem->get_kind() == stmt_kind; }));
    }
  }
}

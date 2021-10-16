#include "catch.hpp"
#include <string>
#include <vector>

#include "follows_parent_handler.h"
#include "pkb.h"
#include "pkb_stub.hpp"

std::string sample = "procedure Example {"
                        "x = 2;"
                        "z = 3;"
                        "i = 5;"
                        "while (i!=0) {"
                        "x = x - 1;"
                        "if (x==1) then {"
                        "z = x + 1; }"
                        "else {"
                        "y = z + x; }"
                        "z = z + x + i;"
                        "call q;"
                        "i = i - 1; } }"

                        "procedure q {"
                        "if (x==1) then {"
                        "z = x + 1; }"
                        "else {"
                        "x = z + x; } }";

//class StatementTableStub : public StmtTable {
// protected:
//  std::map<int, Statement> table_;
//  std::map<NodeType, std::vector<Statement *>> type_to_statement_;
//
// public:
//  void AddStmt(Statement statement) {
//    table_[statement.get_stmt_no()] = statement;
//  }
//
//  std::vector<Statement *> get_all_stmts() {
//    std::vector<Statement *> ans;
//    for (auto &[stmt_no, stmt] : table_) {
//      ans.push_back(&stmt);
//    }
//    return ans;
//  }
//
//  void AssignStmtType() {
//    for (auto &[stmt_no, stmt] : table_) {
//      type_to_statement_[stmt.get_kind()].push_back(&table_[stmt_no]);
//    }
//  }
//};
//
//class PKBStub : public PKB {
// protected:
//  StatementTableStub stmt_table_;
//
// public:
//  void set_stmt_table(StatementTableStub stmt_table) {
//    stmt_table_ = stmt_table;
//  }
//  std::vector<Statement *> get_all_stmts() {
//    return stmt_table_.get_all_stmts();
//  }
//
//  Statement *get_statement(int line_no) override {
//    return stmt_table_.get_stmt(line_no);
//  }
//};
//
//PKBStub *BuildPKBForStatement() {
//  PKBStub *pkb_stub = new PKBStub();
//  StatementTableStub stmt_table = StatementTableStub();
//
//  // Creating Statements and adding to stmt table
//  Statement stmt1 = Statement(1, NodeType::Assign);
//  stmt1.AddFollower(2);
//  stmt1.AddFollowerStar(2);
//  stmt1.AddFollowerStar(3);
//  stmt1.AddFollowerStar(4);
//  stmt1.AddModifies("x");
//  stmt_table.AddStmt(stmt1);
//
//  Statement stmt2 = Statement(2, NodeType::Assign);
//  stmt2.AddFollower(3);
//  stmt2.AddFollowerStar(3);
//  stmt2.AddFollowerStar(4);
//  stmt2.AddFollowee(1);
//  stmt2.AddFolloweeStar(1);
//  stmt2.AddModifies("z");
//  stmt_table.AddStmt(stmt2);
//
//  Statement stmt3 = Statement(3, NodeType::Assign);
//  stmt3.AddFollower(4);
//  stmt3.AddFollowerStar(4);
//  stmt3.AddFollowee(2);
//  stmt3.AddFolloweeStar(1);
//  stmt3.AddFolloweeStar(2);
//  stmt3.AddModifies("i");
//  stmt_table.AddStmt(stmt3);
//
//  Statement stmt4 = Statement(4, NodeType::While);
//  stmt4.AddFollowee(3);
//  stmt4.AddFolloweeStar(1);
//  stmt4.AddFolloweeStar(2);
//  stmt4.AddFolloweeStar(3);
//  stmt4.AddChild(5);
//  stmt4.AddChild(6);
//  stmt4.AddChild(9);
//  stmt4.AddChild(10);
//  stmt4.AddChild(11);
//  stmt4.AddChildStar(5);
//  stmt4.AddChildStar(6);
//  stmt4.AddChildStar(7);
//  stmt4.AddChildStar(8);
//  stmt4.AddChildStar(9);
//  stmt4.AddChildStar(10);
//  stmt4.AddChildStar(11);
//  stmt4.AddModifies("i");
//  stmt4.AddModifies("x");
//  stmt4.AddModifies("y");
//  stmt4.AddModifies("z");
//  stmt4.AddUses("i");
//  stmt4.AddUses("x");
//  stmt4.AddUses("z");
//  stmt_table.AddStmt(stmt4);
//
//  Statement stmt5 = Statement(5, NodeType::Assign);
//  stmt5.AddFollower(6);
//  stmt5.AddFollowerStar(6);
//  stmt5.AddFollowerStar(9);
//  stmt5.AddFollowerStar(10);
//  stmt5.AddFollowerStar(11);
//  stmt5.AddParent(4);
//  stmt5.AddParentStar(4);
//  stmt5.AddModifies("x");
//  stmt5.AddUses("x");
//  stmt_table.AddStmt(stmt5);
//
//  Statement stmt6 = Statement(6, NodeType::If);
//  stmt6.AddFollower(9);
//  stmt6.AddFollowerStar(9);
//  stmt6.AddFollowerStar(10);
//  stmt6.AddFollowerStar(11);
//  stmt6.AddFollowee(5);
//  stmt6.AddFolloweeStar(5);
//  stmt6.AddParent(4);
//  stmt6.AddParentStar(4);
//  stmt6.AddChild(7);
//  stmt6.AddChild(8);
//  stmt6.AddChildStar(7);
//  stmt6.AddChildStar(8);
//  stmt6.AddModifies("y");
//  stmt6.AddModifies("z");
//  stmt6.AddUses("x");
//  stmt6.AddUses("z");
//  stmt_table.AddStmt(stmt6);
//
//  Statement stmt7 = Statement(7, NodeType::Assign);
//  stmt7.AddParent(6);
//  stmt7.AddParentStar(4);
//  stmt7.AddParentStar(6);
//  stmt7.AddModifies("z");
//  stmt7.AddUses("x");
//  stmt_table.AddStmt(stmt7);
//
//  Statement stmt8 = Statement(8, NodeType::Assign);
//  stmt8.AddParent(6);
//  stmt8.AddParentStar(4);
//  stmt8.AddParentStar(6);
//  stmt8.AddModifies("y");
//  stmt8.AddUses("x");
//  stmt8.AddUses("z");
//  stmt_table.AddStmt(stmt8);
//
//  Statement stmt9 = Statement(9, NodeType::Assign);
//  stmt9.AddFollower(10);
//  stmt9.AddFollowerStar(10);
//  stmt9.AddFollowerStar(11);
//  stmt9.AddFollowee(6);
//  stmt9.AddFolloweeStar(5);
//  stmt9.AddFolloweeStar(6);
//  stmt9.AddParent(4);
//  stmt9.AddParentStar(4);
//  stmt9.AddModifies("z");
//  stmt9.AddUses("i");
//  stmt9.AddUses("x");
//  stmt9.AddUses("z");
//  stmt_table.AddStmt(stmt9);
//
//  Statement stmt10 = Statement(10, NodeType::Call);
//  stmt10.AddFollower(11);
//  stmt10.AddFollowerStar(11);
//  stmt10.AddFollowee(9);
//  stmt10.AddFolloweeStar(5);
//  stmt10.AddFolloweeStar(6);
//  stmt10.AddFolloweeStar(9);
//  stmt10.AddParent(4);
//  stmt10.AddParentStar(4);
//  stmt10.AddModifies("x");
//  stmt10.AddModifies("z");
//  stmt10.AddUses("x");
//  stmt10.AddUses("z");
//  stmt_table.AddStmt(stmt10);
//
//  Statement stmt11 = Statement(11, NodeType::Assign);
//  stmt11.AddFollowee(10);
//  stmt11.AddFolloweeStar(5);
//  stmt11.AddFolloweeStar(6);
//  stmt11.AddFolloweeStar(9);
//  stmt11.AddFolloweeStar(10);
//  stmt11.AddParent(4);
//  stmt11.AddParentStar(4);
//  stmt11.AddModifies("i");
//  stmt11.AddUses("i");
//  stmt_table.AddStmt(stmt11);
//
//  Statement stmt12 = Statement(12, NodeType::If);
//  stmt12.AddChild(13);
//  stmt12.AddChild(14);
//  stmt12.AddChildStar(13);
//  stmt12.AddChildStar(14);
//  stmt12.AddModifies("x");
//  stmt12.AddModifies("z");
//  stmt12.AddUses("x");
//  stmt12.AddUses("z");
//  stmt_table.AddStmt(stmt12);
//
//  Statement stmt13 = Statement(13, NodeType::Assign);
//  stmt13.AddParent(12);
//  stmt13.AddParentStar(12);
//  stmt13.AddModifies("z");
//  stmt13.AddUses("x");
//  stmt_table.AddStmt(stmt13);
//
//  Statement stmt14 = Statement(14, NodeType::Assign);
//  stmt14.AddParent(12);
//  stmt14.AddParentStar(12);
//  stmt14.AddModifies("x");
//  stmt14.AddUses("x");
//  stmt14.AddUses("z");
//  stmt_table.AddStmt(stmt14);
//
//  stmt_table.AssignStmtType();
//
//  pkb_stub->set_stmt_table(stmt_table);
//
//  return pkb_stub;
//}

TEST_CASE("FollowsClauses_DifferentNumberOfSyns_OutputsResultTable") {
  SECTION("Follows with 0 synonyms") {
    // Follows(3, 4)
    SuchThatClause follows = SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(3);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, {});
    follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                   &Statement::get_followees);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Follows with s1 as left arg") {
    // Follows(s1, 4)
    SuchThatClause follows = SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                   &Statement::get_followees);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "3");
  }

  SECTION("Follows with s1 as right arg") {
    // Follows(3, s1)
    SuchThatClause follows = SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(3);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                   &Statement::get_followees);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "4");
  }

  SECTION("Follows with s1 and s2 as args") {
    // Follows(s1, s2)
    SuchThatClause follows = SuchThatClause("Follows");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;
    syn_to_entities_vec["s2"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_followers,
                                                   &Statement::get_followees);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 7);
    REQUIRE(res->get_table()->at(0).size() == 2);
    REQUIRE(res->get_table()->at(0).at(0) == "1");
    REQUIRE(res->get_table()->at(0).at(1) == "2");
    REQUIRE(res->get_table()->at(1).at(0) == "2");
    REQUIRE(res->get_table()->at(1).at(1) == "3");
    REQUIRE(res->get_table()->at(2).at(0) == "3");
    REQUIRE(res->get_table()->at(2).at(1) == "4");
    REQUIRE(res->get_table()->at(3).at(0) == "5");
    REQUIRE(res->get_table()->at(3).at(1) == "6");
    REQUIRE(res->get_table()->at(4).at(0) == "6");
    REQUIRE(res->get_table()->at(4).at(1) == "9");
    REQUIRE(res->get_table()->at(5).at(0) == "9");
    REQUIRE(res->get_table()->at(5).at(1) == "10");
    REQUIRE(res->get_table()->at(6).at(0) == "10");
    REQUIRE(res->get_table()->at(6).at(1) == "11");
  }
}

//TEST_CASE("FollowsClauses_AssignAsArgs_OutputsResultTable") {
//  SECTION("Follows with a as left arg") {
//    // Follows(a, 6)
//    SuchThatClause follows = SuchThatClause("Follows");
//    StmtRef left_stmt_ref = StmtRef();
//    left_stmt_ref.set_synonym("a");
//    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
//    StmtRef right_stmt_ref = StmtRef();
//    right_stmt_ref.set_stmt_num(6);
//    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
//    follows.set_ref(left_such_that_ref, right_such_that_ref);
//    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);
//
//    PKBStub *pkb_stub = BuildPKBForStatement();
//    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
//    std::vector<Statement *> entities_stmt = pkb_stub->get_statements(NodeType::Assign);
//    for (Statement *stmt : entities_stmt) {
//      printf("stmt no %d\n", stmt->get_stmt_no());
//    }
//    std::vector<Entity *> entities;
//    for (Statement *stmt : entities_stmt) {
//      auto *entity = static_cast<Entity *>(stmt);
//      entities.push_back(entity);
//    }
//    syn_to_entities_vec["a"] = entities;
//
//    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
//    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
//    follows_parents_handler->set_function_pointers(&Statement::get_followers,
//                                                   &Statement::get_followees);
//    ResultTable *res = follows_parents_handler->Evaluate();
//    REQUIRE(res->get_table()->size() == 1);
//    REQUIRE(res->get_table()->at(0).size() == 1);
//    REQUIRE(res->get_table()->at(0).at(0) == "5");
//  }
//
//  SECTION("Follows with a as right arg") {
//    // Follows(10, a)
//    SuchThatClause follows = SuchThatClause("Follows");
//    StmtRef left_stmt_ref = StmtRef();
//    left_stmt_ref.set_stmt_num(10);
//    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
//    StmtRef right_stmt_ref = StmtRef();
//    right_stmt_ref.set_synonym("a");
//    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
//    follows.set_ref(left_such_that_ref, right_such_that_ref);
//    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);
//
//    PKBStub *pkb_stub = BuildPKBForStatement();
//    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
//    std::vector<Statement *> entities_stmt = pkb_stub->get_statements(NodeType::Assign);
//    for (Statement *stmt : entities_stmt) {
//      printf("right arg stmt no %d\n", stmt->get_stmt_no());
//    }
//    std::vector<Entity *> entities;
//    for (Statement *stmt : entities_stmt) {
//      auto *entity = static_cast<Entity *>(stmt);
//      entities.push_back(entity);
//    }
//    syn_to_entities_vec["a"] = entities;
//
//    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
//    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
//    follows_parents_handler->set_function_pointers(&Statement::get_followers,
//                                                   &Statement::get_followees);
//    ResultTable *res = follows_parents_handler->Evaluate();
//    REQUIRE(res->get_table()->size() == 1);
//    REQUIRE(res->get_table()->at(0).size() == 1);
//    REQUIRE(res->get_table()->at(0).at(0) == "11");
//  }
//
//  SECTION("Follows with a1 and a2 as args") {
//    // Follows(a1, a2)
//    SuchThatClause follows = SuchThatClause("Follows");
//    StmtRef left_stmt_ref = StmtRef();
//    left_stmt_ref.set_synonym("a1");
//    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
//    StmtRef right_stmt_ref = StmtRef();
//    right_stmt_ref.set_synonym("a2");
//    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
//    follows.set_ref(left_such_that_ref, right_such_that_ref);
//    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);
//
//    PKBStub *pkb_stub = BuildPKBForStatement();
//    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
//    std::vector<Statement *> entities_stmt = pkb_stub->get_statements(NodeType::Assign);
//    std::vector<Entity *> entities;
//    for (Statement *stmt : entities_stmt) {
//      auto *entity = static_cast<Entity *>(stmt);
//      entities.push_back(entity);
//    }
//    syn_to_entities_vec["a1"] = entities;
//    syn_to_entities_vec["a2"] = entities;
//
//    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
//    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
//    follows_parents_handler->set_function_pointers(&Statement::get_followers,
//                                                   &Statement::get_followees);
//    ResultTable *res = follows_parents_handler->Evaluate();
//    REQUIRE(res->get_table()->size() == 2);
//    REQUIRE(res->get_table()->at(0).size() == 2);
//    REQUIRE(res->get_table()->at(0).at(0) == "1");
//    REQUIRE(res->get_table()->at(0).at(1) == "2");
//    REQUIRE(res->get_table()->at(1).at(0) == "2");
//    REQUIRE(res->get_table()->at(1).at(1) == "3");
//  }
//}

TEST_CASE("Follows*Clauses_DiferentNumberOfSyns_OutputsResultTable") {
  SECTION("Follows* with 0 synonyms") {
    // Follows*(5, 11)
    SuchThatClause follows = SuchThatClause("Follows*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(5);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(11);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, {});
    follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                   &Statement::get_followees_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Follows* with s1 as left arg") {
    // Follows*(s1, 4)
    SuchThatClause follows = SuchThatClause("Follows*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(4);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                   &Statement::get_followees_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 3);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "1");
    REQUIRE(res->get_table()->at(1).at(0) == "2");
    REQUIRE(res->get_table()->at(2).at(0) == "3");
  }

  SECTION("Follows* with s1 as right arg") {
    // Follows*(6, s1)
    SuchThatClause follows = SuchThatClause("Follows*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(6);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                   &Statement::get_followees_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 3);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "9");
    REQUIRE(res->get_table()->at(1).at(0) == "10");
    REQUIRE(res->get_table()->at(2).at(0) == "11");
  }

  SECTION("Follows with s1 and s2 as args") {
    // Follows*(s1, s2)
    SuchThatClause follows = SuchThatClause("Follows*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;
    syn_to_entities_vec["s2"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_followers_star,
                                                   &Statement::get_followees_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 16);
  }
}

TEST_CASE("ParentClauses_DiferentNumberOfSyns_OutputsResultTable") {
  SECTION("Parent with 0 synonyms") {
    // Parent(4, 6)
    SuchThatClause follows = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(6);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, {});
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Parent with s1 as left arg") {
    // Parent(s1, 7)
    SuchThatClause follows = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(7);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "6");
  }

  SECTION("Parent with s1 as right arg") {
    // Parent(12, s1)
    SuchThatClause follows = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(12);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 2);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "13");
    REQUIRE(res->get_table()->at(1).at(0) == "14");
  }

  SECTION("Parent with s1 and s2 as args") {
    // Parent(s1, s2)
    SuchThatClause follows = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;
    syn_to_entities_vec["s2"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 9);
  }
}

TEST_CASE("Parent*Clauses_DiferentNumberOfSyns_OutputsResultTable") {
  SECTION("Parent* with 0 synonyms") {
    // Parent*(4, 7)
    SuchThatClause follows = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(7);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, {});
    follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                   &Statement::get_parents_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Parent* with s1 as left arg") {
    // Parent*(s1, 7)
    SuchThatClause follows = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(7);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                   &Statement::get_parents_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 2);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "4");
    REQUIRE(res->get_table()->at(1).at(0) == "6");
  }

  SECTION("Parent* with s1 as right arg") {
    // Parent*(4, s1)
    SuchThatClause follows = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                   &Statement::get_parents_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 7);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "5");
    REQUIRE(res->get_table()->at(1).at(0) == "6");
    REQUIRE(res->get_table()->at(2).at(0) == "7");
    REQUIRE(res->get_table()->at(3).at(0) == "8");
    REQUIRE(res->get_table()->at(4).at(0) == "9");
    REQUIRE(res->get_table()->at(5).at(0) == "10");
    REQUIRE(res->get_table()->at(6).at(0) == "11");
  }

  SECTION("Parent* with s1 and s2 as args") {
    // Parent*(s1, s2)
    SuchThatClause follows = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    follows.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> follows_ptr = std::make_shared<SuchThatClause>(follows);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;
    syn_to_entities_vec["s2"] = entities;

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, follows_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                   &Statement::get_parents_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 11);
  }
}

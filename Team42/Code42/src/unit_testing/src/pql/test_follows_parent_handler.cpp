#include "catch.hpp"
#include <string>
#include <vector>

#include "follows_parent_handler.h"
#include "pkb.h"
#include "pkb_stub.hpp"

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
    SuchThatClause parent = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(6);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, parent_ptr, {});
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Parent with s1 as left arg") {
    // Parent(s1, 7)
    SuchThatClause parent = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(7);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

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
    follows_parents_handler->set_args(pkb_stub, parent_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "6");
  }

  SECTION("Parent with s1 as right arg") {
    // Parent(12, s1)
    SuchThatClause parent = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(12);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

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
    follows_parents_handler->set_args(pkb_stub, parent_ptr, syn_to_entities_vec);
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
    SuchThatClause parent = SuchThatClause("Parent");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

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
    follows_parents_handler->set_args(pkb_stub, parent_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children,
                                                   &Statement::get_parents);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 9);
  }
}

TEST_CASE("Parent*Clauses_DiferentNumberOfSyns_OutputsResultTable") {
  SECTION("Parent* with 0 synonyms") {
    // Parent*(4, 7)
    SuchThatClause parent = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(7);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    FollowsParentHandler *follows_parents_handler = FollowsParentHandler::get_instance();
    follows_parents_handler->set_args(pkb_stub, parent_ptr, {});
    follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                   &Statement::get_parents_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Parent* with s1 as left arg") {
    // Parent*(s1, 7)
    SuchThatClause parent = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_stmt_num(7);
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

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
    follows_parents_handler->set_args(pkb_stub, parent_ptr, syn_to_entities_vec);
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
    SuchThatClause parent = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_stmt_num(4);
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s1");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

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
    follows_parents_handler->set_args(pkb_stub, parent_ptr, syn_to_entities_vec);
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
    SuchThatClause parent = SuchThatClause("Parent*");
    StmtRef left_stmt_ref = StmtRef();
    left_stmt_ref.set_synonym("s1");
    SuchThatRef *left_such_that_ref = new SuchThatRef(left_stmt_ref);
    StmtRef right_stmt_ref = StmtRef();
    right_stmt_ref.set_synonym("s2");
    SuchThatRef *right_such_that_ref = new SuchThatRef(right_stmt_ref);
    parent.set_ref(left_such_that_ref, right_such_that_ref);
    std::shared_ptr<SuchThatClause> parent_ptr = std::make_shared<SuchThatClause>(parent);

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
    follows_parents_handler->set_args(pkb_stub, parent_ptr, syn_to_entities_vec);
    follows_parents_handler->set_function_pointers(&Statement::get_children_star,
                                                   &Statement::get_parents_star);
    ResultTable *res = follows_parents_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 11);
  }
}

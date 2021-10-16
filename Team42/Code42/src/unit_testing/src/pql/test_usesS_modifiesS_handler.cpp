#include "catch.hpp"
#include <string>
#include <vector>

#include "usess_modifiess_handler.h"
#include "pkb.h"
#include "pkb_stub.hpp"

TEST_CASE("UsesClauses_DifferentCases_OutputsResultTable") {
  SECTION("Uses with no synonyms") {
    //Uses(7, "x")
    SuchThatClause uses = SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_stmt_num(7);
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_argument("x");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                 &Variable::get_stmts_using);
    uses_modifies_handler->set_args(pkb_stub, uses_ptr, {});
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Uses with s1 as left arg") {
    //Uses(s1, "x")
    SuchThatClause uses = SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_synonym("s1");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_argument("x");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                 &Variable::get_stmts_using);
    uses_modifies_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 10);
    // 4, 5, 6, 7, 8, 9, 10, 12, 13, 14
    REQUIRE(res->get_table()->at(0).at(0) == "4");
    REQUIRE(res->get_table()->at(1).at(0) == "5");
    REQUIRE(res->get_table()->at(2).at(0) == "6");
    REQUIRE(res->get_table()->at(3).at(0) == "7");
    REQUIRE(res->get_table()->at(4).at(0) == "8");
    REQUIRE(res->get_table()->at(5).at(0) == "9");
    REQUIRE(res->get_table()->at(6).at(0) == "10");
    REQUIRE(res->get_table()->at(7).at(0) == "12");
    REQUIRE(res->get_table()->at(8).at(0) == "13");
    REQUIRE(res->get_table()->at(9).at(0) == "14");
  }

  SECTION("Uses with right arg as _") {
    //Uses(10, _)
    SuchThatClause uses = SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_stmt_num(10);
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_wild_card();
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                 &Variable::get_stmts_using);
    uses_modifies_handler->set_args(pkb_stub, uses_ptr, {});
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Uses with left syn s1 and right arg as _") {
    //Uses(s1, _)
    SuchThatClause uses = SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_synonym("s1");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_wild_card();
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                 &Variable::get_stmts_using);
    uses_modifies_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 11);
  }

  SECTION("Uses with v as right arg") {
    //Uses(4, v)
    SuchThatClause uses = SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_stmt_num(4);
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_synonym("v");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Variable *> entities_vars = pkb_stub->get_all_vars();
    std::vector<Entity *> entities;
    for (Variable *var : entities_vars) {
      auto *entity = static_cast<Entity *>(var);
      entities.push_back(entity);
    }
    syn_to_entities_vec["v"] = entities;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                 &Variable::get_stmts_using);
    uses_modifies_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 3);
  }

  SECTION("Uses with s1 and v as left and right args") {
    //Uses(s1, v)
    SuchThatClause uses = SuchThatClause("Uses");
    StmtRef uses_left_stmt = StmtRef();
    uses_left_stmt.set_synonym("s1");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_stmt);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_synonym("v");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Variable *> entities_vars = pkb_stub->get_all_vars();
    std::vector<Entity *> entities_v;
    for (Variable *var : entities_vars) {
      auto *entity = static_cast<Entity *>(var);
      entities_v.push_back(entity);
    }
    syn_to_entities_vec["v"] = entities_v;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities_s;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities_s.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities_s;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_uses,
                                                 &Variable::get_stmts_using);
    uses_modifies_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 20);
  }
}

TEST_CASE("ModifiesClauses_DifferentCases_OutputsResultTable") {
  SECTION("Modifies with no synonyms") {
    //Modifies(11, "i")
    SuchThatClause modifies = SuchThatClause("Modifies");
    StmtRef modifies_left_stmt = StmtRef();
    modifies_left_stmt.set_stmt_num(11);
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_stmt);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_argument("i");
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                 &Variable::get_stmts_modifying);
    uses_modifies_handler->set_args(pkb_stub, modifies_ptr, {});
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Modifies with s1 as left arg") {
    //Modifies(s1, "y")
    SuchThatClause modifies = SuchThatClause("Modifies");
    StmtRef modifies_left_stmt = StmtRef();
    modifies_left_stmt.set_synonym("s1");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_stmt);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_argument("y");
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                 &Variable::get_stmts_modifying);
    uses_modifies_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 3);
    REQUIRE(res->get_table()->at(0).at(0) == "4");
    REQUIRE(res->get_table()->at(1).at(0) == "6");
    REQUIRE(res->get_table()->at(2).at(0) == "8");
  }

  SECTION("Modifies with right arg as _") {
    //Modifies(6, _)
    SuchThatClause modifies = SuchThatClause("Modifies");
    StmtRef modifies_left_stmt = StmtRef();
    modifies_left_stmt.set_stmt_num(6);
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_stmt);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_wild_card();
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                 &Variable::get_stmts_modifying);
    uses_modifies_handler->set_args(pkb_stub, modifies_ptr, {});
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Modifies with left syn s1 and right arg as _") {
    //Modifies(s1, _)
    SuchThatClause modifies = SuchThatClause("Modifies");
    StmtRef modifies_left_stmt = StmtRef();
    modifies_left_stmt.set_synonym("s1");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_stmt);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_wild_card();
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                 &Variable::get_stmts_modifying);
    uses_modifies_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 14);
  }

  SECTION("Modifies with v as right arg") {
    //Modifies(4, v)
    SuchThatClause modifies = SuchThatClause("Modifies");
    StmtRef modifies_left_stmt = StmtRef();
    modifies_left_stmt.set_stmt_num(4);
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_stmt);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_synonym("v");
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Variable *> entities_vars = pkb_stub->get_all_vars();
    std::vector<Entity *> entities;
    for (Variable *var : entities_vars) {
      auto *entity = static_cast<Entity *>(var);
      entities.push_back(entity);
    }
    syn_to_entities_vec["v"] = entities;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                 &Variable::get_stmts_modifying);
    uses_modifies_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 4);
  }

  SECTION("Uses with s1 and v as left and right args") {
    //Modifies(s1, v)
    SuchThatClause modifies = SuchThatClause("Modifies");
    StmtRef modifies_left_stmt = StmtRef();
    modifies_left_stmt.set_synonym("s1");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_stmt);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_synonym("v");
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Variable *> entities_vars = pkb_stub->get_all_vars();
    std::vector<Entity *> entities_v;
    for (Variable *var : entities_vars) {
      auto *entity = static_cast<Entity *>(var);
      entities_v.push_back(entity);
    }
    syn_to_entities_vec["v"] = entities_v;
    std::vector<Statement *> entities_stmt = pkb_stub->get_all_stmts();
    std::vector<Entity *> entities_s;
    for (Statement *stmt : entities_stmt) {
      auto *entity = static_cast<Entity *>(stmt);
      entities_s.push_back(entity);
    }
    syn_to_entities_vec["s1"] = entities_s;

    UsesSModifiesSHandler *uses_modifies_handler = UsesSModifiesSHandler::get_instance();
    uses_modifies_handler->set_function_pointers(&Statement::get_modifies,
                                                 &Variable::get_stmts_modifying);
    uses_modifies_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = uses_modifies_handler->Evaluate();
    REQUIRE(res->get_table()->size() == 20);
  }
}

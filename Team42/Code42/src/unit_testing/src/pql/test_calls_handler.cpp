#include "catch.hpp"
#include <string>
#include <vector>

#include "calls_handler.h"
#include "pkb.h"
#include "pkb_stub.hpp"

TEST_CASE("CallsClauses_DifferentCases_OutputsResultTable") {
  SECTION("Calls with no synonyms") {
    //Calls("Example", "q")
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_argument("Example");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_argument("q");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls with left wildcard and right string") {
    //Calls(_, "q")
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_wild_card();
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_argument("q");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls with left string and right wildcard") {
    //Calls("Example", _)
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_argument("Example");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_wild_card();
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls with left synonym p and right wildcard") {
    //Calls(p, _)
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_synonym("p");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_wild_card();
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
  }

  SECTION("Calls with left wildcard and right synonym p") {
    //Calls(_, p)
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_wild_card();
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_synonym("p");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "q");
  }

  SECTION("Calls with both wildcards") {
    //Calls(_, _)
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_wild_card();
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_wild_card();
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls with left synonym p") {
    //Calls(p, "q")
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_synonym("p");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_argument("q");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
  }

  SECTION("Calls with right synonym p") {
    //Calls("Example", p)
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_argument("Example");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_synonym("p");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "q");
  }

  SECTION("Calls with synonyms p and p1") {
    //Calls(p, p1)
    SuchThatClause calls = SuchThatClause("Calls");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_synonym("p");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_synonym("p1");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;
    syn_to_entities_vec["p1"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 2);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
    REQUIRE(res->get_table()->at(0).at(1) == "q");
  }
}

TEST_CASE("Calls*Clauses_DifferentCases_OutputsResultTable") {
  SECTION("Calls* with no synonyms") {
    //Calls*("Example", "q")
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_argument("Example");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_argument("q");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls* with left wildcard and right string") {
    //Calls*(_, "q")
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_wild_card();
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_argument("q");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls* with left string and right wildcard") {
    //Calls*("Example", _)
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_argument("Example");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_wild_card();
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls* with left synonym p and right wildcard") {
    //Calls*(p, _)
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_synonym("p");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_wild_card();
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
  }

  SECTION("Calls* with left wildcard and right synonym p") {
    //Calls*(_, p)
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_wild_card();
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_synonym("p");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "q");
  }

  SECTION("Calls* with both wildcards") {
    //Calls*(_, _)
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_wild_card();
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_wild_card();
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, {});
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Calls* with left synonym p") {
    //Calls*(p, "q")
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_synonym("p");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_argument("q");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
  }

  SECTION("Calls* with right synonym p") {
    //Calls*("Example", p)
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_argument("Example");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_synonym("p");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "q");
  }

  SECTION("Calls* with synonyms p and p1") {
    //Calls*(p, p1)
    SuchThatClause calls = SuchThatClause("Calls*");
    EntRef calls_left_proc = EntRef();
    calls_left_proc.set_synonym("p");
    SuchThatRef *calls_left_such_that = new SuchThatRef(calls_left_proc);
    EntRef calls_right_proc = EntRef();
    calls_right_proc.set_synonym("p1");
    SuchThatRef *calls_right_such_that = new SuchThatRef(calls_right_proc);
    calls.set_ref(calls_left_such_that, calls_right_such_that);
    std::shared_ptr<SuchThatClause> calls_ptr = std::make_shared<SuchThatClause>(calls);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;
    syn_to_entities_vec["p1"] = entities;

    CallsHandler *calls_handler = CallsHandler::get_instance();
    calls_handler->set_args(pkb_stub, calls_ptr, syn_to_entities_vec);
    ResultTable *res = calls_handler->EvaluateCalls();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).size() == 2);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
    REQUIRE(res->get_table()->at(0).at(1) == "q");
  }
}

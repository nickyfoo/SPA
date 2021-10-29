#include "catch.hpp"
#include <string>
#include <vector>

#include "usesp_modifiesp_handler.h"
#include "pkb.h"
#include "pkb_stub.hpp"

TEST_CASE("UsesPClauses_DifferentCases_OutputsResultTable") {
  SECTION("Uses with no synonyms") {
    //Uses("Example", "i")
    SuchThatClause uses = SuchThatClause("Uses");
    EntRef uses_left_proc = EntRef();
    uses_left_proc.set_argument("Example");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_proc);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_argument("i");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, uses_ptr, {});
    ResultTable *res = usep_modifiesp_handler->EvaluateUsesP();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Uses with p as left arg") {
    //Uses(p, "x")
    SuchThatClause uses = SuchThatClause("Uses");
    EntRef uses_left_proc = EntRef();
    uses_left_proc.set_synonym("p");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_proc);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_argument("x");
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateUsesP();
    REQUIRE(res->get_table()->size() == 2);
    REQUIRE(res->get_table()->at(0).size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
    REQUIRE(res->get_table()->at(1).at(0) == "q");
  }

  SECTION("Uses with right arg as _") {
    //Uses("q", _)
    SuchThatClause uses = SuchThatClause("Uses");
    EntRef uses_left_proc = EntRef();
    uses_left_proc.set_argument("q");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_proc);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_wild_card();
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, uses_ptr, {});
    ResultTable *res = usep_modifiesp_handler->EvaluateUsesP();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Uses with left syn p and right arg as _") {
    //Uses(p, _)
    SuchThatClause uses = SuchThatClause("Uses");
    EntRef uses_left_proc = EntRef();
    uses_left_proc.set_synonym("p");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_proc);
    EntRef uses_right_name = EntRef();
    uses_right_name.set_wild_card();
    SuchThatRef *uses_right_such_that = new SuchThatRef(uses_right_name);
    uses.set_ref(uses_left_such_that, uses_right_such_that);
    std::shared_ptr<SuchThatClause> uses_ptr = std::make_shared<SuchThatClause>(uses);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateUsesP();
    REQUIRE(res->get_table()->size() == 2);
    REQUIRE(res->get_table()->at(0).size() == 1);
  }

  SECTION("Uses with v as right arg") {
    //Uses("Example", v)
    SuchThatClause uses = SuchThatClause("Uses");
    EntRef uses_left_proc = EntRef();
    uses_left_proc.set_argument("Example");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_proc);
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

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateUsesP();
    REQUIRE(res->get_table()->size() == 3);
    REQUIRE(res->get_table()->at(0).size() == 1);
  }

  SECTION("Uses with p and v as left and right args") {
    //Uses(p, v)
    SuchThatClause uses = SuchThatClause("Uses");
    EntRef uses_left_proc = EntRef();
    uses_left_proc.set_synonym("p");
    SuchThatRef *uses_left_such_that = new SuchThatRef(uses_left_proc);
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
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities_p;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities_p.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities_p;

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, uses_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateUsesP();
    REQUIRE(res->get_table()->size() == 5);
    REQUIRE(res->get_table()->at(0).size() == 2);
  }
}

TEST_CASE("ModifiesPClauses_DifferentCases_OutputsResultTable") {
  SECTION("Modifies with no synonyms") {
    //Modifies("Example", "i")
    SuchThatClause modifies = SuchThatClause("Modifies");
    EntRef modifies_left_proc = EntRef();
    modifies_left_proc.set_argument("Example");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_proc);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_argument("i");
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, modifies_ptr, {});
    ResultTable *res = usep_modifiesp_handler->EvaluateModifiesP();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Modifies with p as left arg") {
    //Modifies(p, "i")
    SuchThatClause modifies = SuchThatClause("Modifies");
    EntRef modifies_left_proc = EntRef();
    modifies_left_proc.set_synonym("p");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_proc);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_argument("i");
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateModifiesP();
    REQUIRE(res->get_table()->size() == 1);
    REQUIRE(res->get_table()->at(0).at(0) == "Example");
    REQUIRE(res->get_table()->at(0).size() == 1);
  }

  SECTION("Modifies with right arg as _") {
    //Modifies("q", _)
    SuchThatClause modifies = SuchThatClause("Modifies");
    EntRef modifies_left_proc = EntRef();
    modifies_left_proc.set_argument("q");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_proc);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_wild_card();
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, modifies_ptr, {});
    ResultTable *res = usep_modifiesp_handler->EvaluateModifiesP();
    REQUIRE(res->get_table()->empty() == true);
  }

  SECTION("Modifies with left syn p and right arg as _") {
    //Modifies(p, _)
    SuchThatClause modifies = SuchThatClause("Modifies");
    EntRef modifies_left_proc = EntRef();
    modifies_left_proc.set_synonym("p");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_proc);
    EntRef modifies_right_name = EntRef();
    modifies_right_name.set_wild_card();
    SuchThatRef *modifies_right_such_that = new SuchThatRef(modifies_right_name);
    modifies.set_ref(modifies_left_such_that, modifies_right_such_that);
    std::shared_ptr<SuchThatClause> modifies_ptr = std::make_shared<SuchThatClause>(modifies);

    PKBStub *pkb_stub = PKBStub::BuildPKB();
    std::unordered_map<std::string, std::vector<Entity *>> syn_to_entities_vec;
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities;

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateModifiesP();
    REQUIRE(res->get_table()->size() == 2);
    REQUIRE(res->get_table()->at(0).size() == 1);
  }

  SECTION("Modifies with v as right arg") {
    //Modifies("q", v)
    SuchThatClause modifies = SuchThatClause("Modifies");
    EntRef modifies_left_proc = EntRef();
    modifies_left_proc.set_argument("q");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_proc);
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

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateModifiesP();
    REQUIRE(res->get_table()->size() == 2);
    REQUIRE(res->get_table()->at(0).size() == 1);
  }

  SECTION("Uses with p and v as left and right args") {
    //Modifies(p, v)
    SuchThatClause modifies = SuchThatClause("Modifies");
    EntRef modifies_left_proc = EntRef();
    modifies_left_proc.set_synonym("p");
    SuchThatRef *modifies_left_such_that = new SuchThatRef(modifies_left_proc);
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
    std::vector<Procedure *> entities_proc = pkb_stub->get_all_procedures();
    std::vector<Entity *> entities_p;
    for (Procedure *proc : entities_proc) {
      auto *entity = static_cast<Entity *>(proc);
      entities_p.push_back(entity);
    }
    syn_to_entities_vec["p"] = entities_p;

    UsesPModifiesPHandler *usep_modifiesp_handler = UsesPModifiesPHandler::get_instance();
    usep_modifiesp_handler->set_args(pkb_stub, modifies_ptr, syn_to_entities_vec);
    ResultTable *res = usep_modifiesp_handler->EvaluateModifiesP();
    REQUIRE(res->get_table()->size() == 6);
    REQUIRE(res->get_table()->at(0).size() == 2);
  }
}

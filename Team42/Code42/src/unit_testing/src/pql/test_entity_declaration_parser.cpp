#include "entity_declaration_parser.h"
#include "catch.hpp"
#include "unordered_map"

using namespace std;

TEST_CASE("Test 1: Correctly adding entities to entities_map") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities = new vector<string>({"stmt s1", "stmt s2"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"s1", new EntityDeclaration(EntityType::Stmt, "s1")},
      {"s2", new EntityDeclaration(EntityType::Stmt, "s2")}
  };
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();
  REQUIRE(expected_entities_map.size() == entities_map->size());
  REQUIRE(expected_entities_map.at("s1")->get_synonym() ==
      entities_map->at("s1")->get_synonym());
  REQUIRE(entities_map->at("s1")->get_type() == EntityType::Stmt);
  REQUIRE(expected_entities_map.at("s2")->get_synonym() ==
      entities_map->at("s2")->get_synonym());
  REQUIRE(entities_map->at("s2")->get_type() == EntityType::Stmt);
}

TEST_CASE("Test 2: Correctly adding entities to entities_map") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities = new vector<string>(
      {"assign assign", "procedure procedure"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"assign", new EntityDeclaration(
          EntityType::Assign, "assign")},
      {"procedure", new EntityDeclaration(
          EntityType::Procedure, "procedure")}
  };
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();
  REQUIRE(expected_entities_map.size() == entities_map->size());
  REQUIRE(expected_entities_map.at("assign")->get_synonym() ==
      entities_map->at("assign")->get_synonym());
  REQUIRE(entities_map->at("assign")->get_type() == EntityType::Assign);
  REQUIRE(expected_entities_map.at("procedure")->get_synonym() ==
      entities_map->at("procedure")->get_synonym());
  REQUIRE(entities_map->at("procedure")->get_type() == EntityType::Procedure);
}

TEST_CASE("Test 3: Having multiple synonyms for 1 entity type_") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities =
      new vector<string>({"stmt S, s2", "procedure p"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"S", new EntityDeclaration(EntityType::Stmt, "S")},
      {"s2", new EntityDeclaration(EntityType::Stmt, "s2")},
      {"p", new EntityDeclaration(EntityType::Procedure, "p")}
  };
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();
  REQUIRE(expected_entities_map.size() == entities_map->size());
  REQUIRE(expected_entities_map.at("S")->get_synonym() ==
      entities_map->at("S")->get_synonym());
  REQUIRE(entities_map->at("S")->get_type() == EntityType::Stmt);
  REQUIRE(expected_entities_map.at("p")->get_synonym() ==
      entities_map->at("p")->get_synonym());
  REQUIRE(entities_map->at("p")->get_type() == EntityType::Procedure);
  REQUIRE(expected_entities_map.at("s2")->get_synonym() ==
      entities_map->at("s2")->get_synonym());
  REQUIRE(entities_map->at("s2")->get_type() == EntityType::Stmt);
}

TEST_CASE("Test 4: Invalid test: invalid entity") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities = new vector<string>({"stmts s", "procedure p"});
  entity_parser->set_entities(entities);
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();

  REQUIRE(entities_map == nullptr);
}

TEST_CASE("Test 5: Invalid test: invalid synonym") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities = new vector<string>({"stmt s?", "procedure p"});
  entity_parser->set_entities(entities);
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();

  REQUIRE(entities_map == nullptr);
}

TEST_CASE("Invalid test: repeat synonym name") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities =
      new vector<string>({"stmt s1, s1", "procedure s"});
  entity_parser->set_entities(entities);
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();

  REQUIRE(entities_map == nullptr);
}

TEST_CASE("Missing entity declaration") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities =
      new vector<string>({"assign"});
  entity_parser->set_entities(entities);
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();
  REQUIRE(entities_map == nullptr);
}

TEST_CASE("Extra space entity declaration") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  vector<string> *entities =
      new vector<string>({"while        while", "call call"});
  entity_parser->set_entities(entities);
  unordered_map<string, EntityDeclaration *> *entities_map =
      entity_parser->get_entities_map();
  REQUIRE(entities_map != nullptr);
}

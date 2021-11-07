#include "entity_declaration_parser.h"
#include "catch.hpp"
#include "unordered_map"

using namespace std;

TEST_CASE("AddingEntitiesToMap_StatementEntities_ReturnsEntitiesMap") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities = new vector<string>({"stmt s1", "stmt s2"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"s1", new EntityDeclaration(EntityType::Stmt, "s1")},
      {"s2", new EntityDeclaration(EntityType::Stmt, "s2")}
  };

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  std::unordered_map<std::string, EntityDeclaration *> *entities_map = std::get<0>(entities_parser_tuple);
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == true);
  REQUIRE(semantically_valid == true);
  REQUIRE(expected_entities_map.size() == entities_map->size());
  REQUIRE(expected_entities_map.at("s1")->get_synonym() ==
      entities_map->at("s1")->get_synonym());
  REQUIRE(entities_map->at("s1")->get_type() == EntityType::Stmt);
  REQUIRE(expected_entities_map.at("s2")->get_synonym() ==
      entities_map->at("s2")->get_synonym());
  REQUIRE(entities_map->at("s2")->get_type() == EntityType::Stmt);
}

TEST_CASE("AddingEntitiesToMap_OtherEntityTypes_ReturnsEntitiesMap") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities = new vector<string>(
      {"assign assign", "procedure procedure"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"assign", new EntityDeclaration(
          EntityType::Assign, "assign")},
      {"procedure", new EntityDeclaration(
          EntityType::Procedure, "procedure")}
  };
  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  std::unordered_map<std::string, EntityDeclaration *> *entities_map = std::get<0>(entities_parser_tuple);
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == true);
  REQUIRE(semantically_valid == true);
  REQUIRE(expected_entities_map.size() == entities_map->size());
  REQUIRE(expected_entities_map.at("assign")->get_synonym() ==
      entities_map->at("assign")->get_synonym());
  REQUIRE(entities_map->at("assign")->get_type() == EntityType::Assign);
  REQUIRE(expected_entities_map.at("procedure")->get_synonym() ==
      entities_map->at("procedure")->get_synonym());
  REQUIRE(entities_map->at("procedure")->get_type() == EntityType::Procedure);
}

TEST_CASE("AddingEntitiesToMap_MultipleSynonymsForOneEntityType_ReturnsEntitiesMap") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"stmt S, s2", "procedure p"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"S", new EntityDeclaration(EntityType::Stmt, "S")},
      {"s2", new EntityDeclaration(EntityType::Stmt, "s2")},
      {"p", new EntityDeclaration(EntityType::Procedure, "p")}
  };

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  std::unordered_map<std::string, EntityDeclaration *> *entities_map = std::get<0>(entities_parser_tuple);
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == true);
  REQUIRE(semantically_valid == true);
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

TEST_CASE("AddingEntitiesToMap_MultipleSynonymsWithNoSpace_ReturnsEntitiesMap") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"stmt S,s2", "procedure p1,p2"});
  entity_parser->set_entities(entities);

  unordered_map<string, EntityDeclaration *> expected_entities_map = {
      {"S", new EntityDeclaration(EntityType::Stmt, "S")},
      {"s2", new EntityDeclaration(EntityType::Stmt, "s2")},
      {"p1", new EntityDeclaration(EntityType::Procedure, "p1")},
      {"p2", new EntityDeclaration(EntityType::Procedure, "p2")}
  };

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  std::unordered_map<std::string, EntityDeclaration *> *entities_map = std::get<0>(entities_parser_tuple);
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == true);
  REQUIRE(semantically_valid == true);
  REQUIRE(expected_entities_map.size() == entities_map->size());
  REQUIRE(expected_entities_map.at("S")->get_synonym() ==
  entities_map->at("S")->get_synonym());
  REQUIRE(entities_map->at("S")->get_type() == EntityType::Stmt);
  REQUIRE(expected_entities_map.at("p1")->get_synonym() ==
  entities_map->at("p1")->get_synonym());
  REQUIRE(entities_map->at("p1")->get_type() == EntityType::Procedure);
  REQUIRE(expected_entities_map.at("p2")->get_synonym() ==
  entities_map->at("p2")->get_synonym());
  REQUIRE(entities_map->at("p2")->get_type() == EntityType::Procedure);
  REQUIRE(expected_entities_map.at("s2")->get_synonym() ==
  entities_map->at("s2")->get_synonym());
  REQUIRE(entities_map->at("s2")->get_type() == EntityType::Stmt);
}

TEST_CASE("InvalidEntityDeclaration_WrongEntityType_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities = new vector<string>({"stmts s", "procedure p"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == false);
  REQUIRE(semantically_valid == true);
}

TEST_CASE("InvalidEntityDeclaration_InvalidSynonym_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities = new vector<string>({"stmt s?", "procedure p"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == false);
  REQUIRE(semantically_valid == true);
}

TEST_CASE("InvalidEntityDeclaration_RepeatSynonym_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"stmt s1, s1", "procedure s"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple;
  entities_parser_tuple = entity_parser->get_entities_map();

  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == true);
  REQUIRE(semantically_valid == false);
}

TEST_CASE("InvalidEntityDeclaration_MissingSynonym_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"assign"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == false);
  REQUIRE(semantically_valid == true);
}

TEST_CASE("AddingEntitiesToMap_ExtraSpaceInEntityDeclaration_ReturnsEntitiesMap") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"while        while", "call call"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == true);
  REQUIRE(semantically_valid == true);
}

TEST_CASE("InvalidSynonym_MissingCommaBetweenMultipleDeclaration_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"stmt s, stmt s1", "call call"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
             bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == false);
  REQUIRE(semantically_valid == true);
}

TEST_CASE("InvalidDeclaration_ExtraCommaAtTheFront_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"stmt ,   s"});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == false);
  REQUIRE(semantically_valid == true);
}

TEST_CASE("InvalidDeclaration_ExtraCommaAtTheBack_ReturnsNullptr") {
  EntityDeclarationParser *entity_parser =
      EntityDeclarationParser::get_instance();
  auto *entities =
      new vector<string>({"stmt s   , "});
  entity_parser->set_entities(entities);

  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> entities_parser_tuple = entity_parser->get_entities_map();
  bool syntactically_valid = std::get<1>(entities_parser_tuple);
  bool semantically_valid = std::get<2>(entities_parser_tuple);

  REQUIRE(syntactically_valid == false);
  REQUIRE(semantically_valid == true);
}

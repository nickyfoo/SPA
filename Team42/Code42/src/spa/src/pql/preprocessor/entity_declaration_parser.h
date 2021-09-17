#pragma once

#include "entity_declaration.h"
#include <vector>
#include <unordered_map>
#include <string>

class EntityDeclarationParser {
 private:
  std::vector<std::string> *entity_declaration_strings_;
  static EntityDeclarationParser *instance_;
  EntityDeclarationParser();
  bool IsValidIdentifier(std::string str);
  EntityType CheckEntityType(std::string type);
  bool IsValidSynonym(std::vector<std::string> tokens,
                      std::vector<std::string> *synonyms);
  std::vector<std::string> SplitString(std::string str,
                                       std::string delimiter);
  bool AddToEntitiesMap(
      std::unordered_map<std::string, EntityDeclaration *> *entities_map,
      std::vector<std::string> *synonyms,
      EntityType entity_type);
  std::string trim(std::string str);

 public:
  static EntityDeclarationParser *get_instance();
  void set_entities(std::vector<std::string> *entities);
  std::unordered_map<std::string, EntityDeclaration *> *get_entities_map();
};

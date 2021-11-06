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
  static bool IsValidIdentifier(std::string str);
  static EntityType CheckEntityType(std::string type);
  bool IsValidSynonym(std::vector<std::string> tokens,
                      std::vector<std::string> *synonyms);
  static std::vector<std::string> SplitString(std::string str,
                                              std::string delimiter);
  static bool AddToEntitiesMap(
      std::unordered_map<std::string, EntityDeclaration *> *entities_map,
      std::vector<std::string> *synonyms,
      EntityType entity_type);
  static std::string trim(std::string str);
  bool syntactically_valid_ = true;
  bool semantically_valid_ = true;

 public:
  static EntityDeclarationParser *get_instance();
  void set_entities(std::vector<std::string> *entities);
  std::tuple<std::unordered_map<std::string, EntityDeclaration *> *,
  bool, bool> get_entities_map();
};

#pragma once
#include <string>
#include <vector>
#include "entity_declaration.h"

enum class ReturnType {
  Name,
  Integer,
  Boolean,
  Default
};

class ResultClause {
 public:
  ResultClause(std::string synonym, EntityType synonym_type, ReturnType return_type);
  ~ResultClause();
  std::string get_synonym();
  EntityType get_synonym_type();
  ReturnType get_return_type();
  std::string get_elem();
 private:
  std::string synonym_;
  EntityType synonym_type_;
  ReturnType return_type_;
  std::string elem_;
  void MakeElem();
};

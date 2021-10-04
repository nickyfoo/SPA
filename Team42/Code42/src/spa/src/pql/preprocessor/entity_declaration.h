#pragma once
#include <string>

enum class EntityType {
  Stmt,
  Read,
  Print,
  Call,
  While,
  If,
  Assign,
  Variable,
  Constant,
  Procedure,
  ProgLine,
  None
};

class EntityDeclaration {
 public:
  EntityDeclaration(EntityType type, std::string synonym);
  EntityType get_type();
  std::string get_synonym();
 private:
  EntityType type_;
  std::string synonym_;
};

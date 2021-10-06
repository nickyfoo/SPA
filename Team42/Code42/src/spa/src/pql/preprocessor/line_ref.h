#pragma once
#include <string>
#include "entity_declaration.h"

enum class LineRefType {
  StmtNum,
  Synonym,
  WildCard,
  None
};

class LineRef {
 public:
  LineRef();
  void set_synonym(const std::string &syn);
  void set_line_num(int line_num);
  void set_wild_card();
  void set_entity_type(EntityType type);

  LineRefType get_type();
  EntityType get_entity_type();
  std::string get_synonym();
  int get_line_num() const;
  std::string get_value();

 private:
  LineRefType type_;
  EntityType entity_type_;
  std::string synonym_;
  int line_num_;
};

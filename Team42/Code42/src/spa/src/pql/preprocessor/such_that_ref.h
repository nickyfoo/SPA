#pragma once
#include "ent_ref.h"
#include "stmt_ref.h"
#include "line_ref.h"

enum class SuchThatRefType {
  Entity,
  Statement,
  Line,
  None
};

class SuchThatRef {
 public:
  SuchThatRef();
  explicit SuchThatRef(EntRef entity_ref);
  explicit SuchThatRef(StmtRef stmt_ref);
  explicit SuchThatRef(LineRef line_ref);

  SuchThatRefType get_type();
  EntRef get_ent_ref();
  StmtRef get_stmt_ref();
  LineRef get_line_ref();

  void set_ent_ref(EntRef entity_ref);
  void set_stmt_ref(StmtRef stmt_ref);
  void set_line_ref(LineRef line_ref);

 private:
  SuchThatRefType type_;
  EntRef entity_ref_;
  StmtRef stmt_ref_;
  LineRef line_ref_;
};

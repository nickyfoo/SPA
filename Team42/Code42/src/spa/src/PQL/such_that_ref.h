#pragma once
#include "ent_ref.h"
#include "stmt_ref.h"

enum class SuchThatRefType {
    Entity,
    Statement,
    None
};

class SuchThatRef {
 public:
    SuchThatRef();
    explicit SuchThatRef(EntRef entity_ref);
    explicit SuchThatRef(StmtRef stmt_ref);

    SuchThatRefType get_type();
    EntRef get_ent_ref();
    StmtRef get_stmt_ref();

 private:
    SuchThatRefType type_;
    EntRef entity_ref_;
    StmtRef stmt_ref_;
};

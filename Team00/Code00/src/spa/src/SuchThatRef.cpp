#include "SuchThatRef.h"

SuchThatRef::SuchThatRef() {
    this->type = SuchThatRefType::None;
}

SuchThatRef::SuchThatRef(EntRef entity_ref) {
    this->type = SuchThatRefType::Entity;
    this->entity_ref = entity_ref;
}

SuchThatRef::SuchThatRef(StmtRef stmt_ref) {
    this->type = SuchThatRefType::Statement;
    this->stmt_ref = stmt_ref;
}

SuchThatRefType SuchThatRef::getType() {
    return this->type;
}

EntRef SuchThatRef::getEntRef() {
    return this->entity_ref;
}

StmtRef SuchThatRef::getStmtRef() {
    return this->stmt_ref;
}
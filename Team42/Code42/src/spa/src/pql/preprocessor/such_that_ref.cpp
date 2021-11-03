#include "such_that_ref.h"

SuchThatRef::SuchThatRef() {
  this->type_ = SuchThatRefType::None;
}

SuchThatRef::SuchThatRef(EntRef entity_ref) {
  this->type_ = SuchThatRefType::Entity;
  this->entity_ref_ = entity_ref;
}

SuchThatRef::SuchThatRef(StmtRef stmt_ref) {
  this->type_ = SuchThatRefType::Statement;
  this->stmt_ref_ = stmt_ref;
}

SuchThatRef::SuchThatRef(LineRef line_ref) {
  this->type_ = SuchThatRefType::Line;
  this->line_ref_ = line_ref;
}

void SuchThatRef::set_ent_ref(EntRef entity_ref) {
  this->type_ = SuchThatRefType::Entity;
  this->entity_ref_ = entity_ref;
}

void SuchThatRef::set_stmt_ref(StmtRef stmt_ref) {
  this->type_ = SuchThatRefType::Statement;
  this->stmt_ref_ = stmt_ref;
}

void SuchThatRef::set_line_ref(LineRef line_ref) {
  this->type_ = SuchThatRefType::Line;
  this->line_ref_ = line_ref;
}

SuchThatRefType SuchThatRef::get_type() {
  return this->type_;
}

EntRef SuchThatRef::get_ent_ref() {
  return this->entity_ref_;
}

StmtRef SuchThatRef::get_stmt_ref() {
  return this->stmt_ref_;
}

LineRef SuchThatRef::get_line_ref() {
  return this->line_ref_;
}

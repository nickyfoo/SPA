#include "usesp_modifiesp_handler.h"

UsesPModifiesPHandler::UsesPModifiesPHandler() = default;
UsesPModifiesPHandler *UsesPModifiesPHandler::instance_ = 0;

UsesPModifiesPHandler *UsesPModifiesPHandler::get_instance() {
  if (!instance_) {
    instance_ = new UsesPModifiesPHandler();
  }
  return instance_;
}

void UsesPModifiesPHandler::set_args(PKB *pkb,
                                     std::unordered_map<std::string, std::vector<Entity *>>
                                     *synonym_to_entity_result,
                                     SuchThatClause *relationship,
                                     std::vector<std::string> *entities_to_return) {
  this->pkb_ = pkb;
  this->synonym_to_entity_result_ = synonym_to_entity_result;
  this->relationship_ = relationship;
  this->entities_to_return_ = entities_to_return;
}

std::set<std::string> *UsesPModifiesPHandler::ProcedureForwarder(
    std::set<std::string> *(Procedure::*function)(),
    Procedure *proc) {
  return (proc->*function)();
}

std::set<std::string> *UsesPModifiesPHandler::VariableForwarder(
    std::set<std::string> *(Variable::*function)(),
    Variable *var) {
  return (var->*function)();
}

void UsesPModifiesPHandler::set_function_pointers(
    std::set<std::string> *(Procedure::*get_normal)(),
    std::set<std::string> *(Variable::*get_reverse)()) {
  this->get_normal_ = get_normal;
  this->get_reverse_ = get_reverse;
}

void UsesPModifiesPHandler::Evaluate() {
  EntRef left_ent = relationship_->get_left_ref()->get_ent_ref();
  EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();

  // Going through 6 different cases for UsesS
  if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::Synonym) {  // UsesP(p, v)
    std::string left_synonym = left_ent.get_synonym();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
    for (int i = 0; i < left_entity_vec->size(); i++) {
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec->at(i));
      if (ProcedureForwarder(get_normal_, proc)->empty()) {
        // Remove procedures that do not have something it uses.
        left_entity_vec->erase(left_entity_vec->begin() + i);
        i--;
      }
    }
    for (int j = 0; j < right_entity_vec->size(); j++) {
      auto *variable = dynamic_cast<Variable *>(right_entity_vec->at(j));
      if (VariableForwarder(get_reverse_, variable)->empty()) {
        // Remove variables that are not used
        right_entity_vec->erase(right_entity_vec->begin() + j);
        j--;
      }
    }
  } else if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::WildCard) {  // UsesP(p, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    for (int i = 0; i < left_entity_vec->size(); i++) {
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec->at(i));
      // Remove each procedure that doesnt use anything.
      if (ProcedureForwarder(get_normal_, proc)->empty()) {
        left_entity_vec->erase(left_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::Argument) {  // UsesP(p, "x")
    std::string left_synonym = left_ent.get_synonym();
    std::string right_arg = right_ent.get_argument();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    for (int i = 0; i < left_entity_vec->size(); i++) {
      auto *stmt = dynamic_cast<Procedure *>(left_entity_vec->at(i));
      // Remove each procedure that doesnt have right arg in its uses
      if (!ProcedureForwarder(get_normal_, stmt)->count(right_arg)) {
        left_entity_vec->erase(left_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::Synonym) {  // UsesP("sth", v)
    std::string left_arg = left_ent.get_argument();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);
    for (int i = 0; i < right_entity_vec->size(); i++) {
      auto *variable = dynamic_cast<Variable *>(right_entity_vec->at(i));
      // Remove each variable that doesnt have left arg in its users.
      if (!VariableForwarder(get_reverse_, variable)->count(left_arg)) {
        right_entity_vec->erase(right_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::WildCard) {  // UsesP("sth", _)
    std::string left_arg = left_ent.get_argument();
    Procedure *proc = pkb_->get_procedure(left_arg);
    if (ProcedureForwarder(get_normal_, proc)->empty()) {
      // If procedure with left arg as name
      // does not use anything then clear results vector
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::Argument) {  // UsesP("sth", "x")
    std::string left_arg = left_ent.get_argument();
    std::string right_arg = right_ent.get_argument();
    Procedure *proc = pkb_->get_procedure(left_arg);
    if (!ProcedureForwarder(get_normal_, proc)->count(right_arg)) {
      // Clear results vector if this relationship_ is false
      synonym_to_entity_result_->at(entities_to_return_->at(0)).clear();
    }
  }
}

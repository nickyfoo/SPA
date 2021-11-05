#include "usesp_modifiesp_handler.h"
#include <assert.h>

UsesPModifiesPHandler::UsesPModifiesPHandler() = default;
UsesPModifiesPHandler *UsesPModifiesPHandler::instance_ = nullptr;

UsesPModifiesPHandler *UsesPModifiesPHandler::get_instance() {
  if (!instance_) {
    instance_ = new UsesPModifiesPHandler();
  }
  return instance_;
}

void UsesPModifiesPHandler::set_args(PKB *pkb,
                                     std::shared_ptr<SuchThatClause> relationship,
                                     std::unordered_map<std::string,
                                     std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
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

ResultTable *UsesPModifiesPHandler::Evaluate() {
  ResultTable *ret = new ResultTable();
  EntRef left_ent = relationship_->get_left_ref()->get_ent_ref();
  EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();
  // Going through 6 different cases for UsesP
  if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::Synonym) {  // UsesP(p, v)
    std::string left_synonym = left_ent.get_synonym();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<std::string> left_proc_vec;
    std::vector<std::string> right_var_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec.at(i));
      for (int j = 0; j < right_entity_vec.size(); j++) {
        auto *variable = dynamic_cast<Variable *>(right_entity_vec.at(j));
        if (proc != nullptr && variable != nullptr
        && ProcedureForwarder(get_normal_, proc)->count(variable->get_name())) {
          left_proc_vec.push_back(proc->get_name());
          right_var_vec.push_back(variable->get_name());
        }
      }
    }
    ret->AddDoubleColumns(left_synonym, left_proc_vec, right_synonym, right_var_vec);
  } else if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::WildCard) {  // UsesP(p, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> left_proc_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec.at(i));
      if (proc != nullptr && !ProcedureForwarder(get_normal_, proc)->empty()) {
        left_proc_vec.push_back(proc->get_name());
      }
    }
    ret->AddSingleColumn(left_synonym, left_proc_vec);
  } else if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::Argument) {  // UsesP(p, "x")
    std::string left_synonym = left_ent.get_synonym();
    std::string right_arg = right_ent.get_argument();
    std::vector<Entity *> left_entity_vec;
    left_entity_vec = synonym_to_entities_vec_.at(left_synonym);
    std::vector<std::string> left_proc_vec;

    for (int i = 0; i < left_entity_vec.size(); i++) {
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec.at(i));
      if (proc != nullptr && ProcedureForwarder(get_normal_, proc)->count(right_arg)) {
        left_proc_vec.push_back(proc->get_name());
      }
    }
    ret->AddSingleColumn(left_synonym, left_proc_vec);
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::Synonym) {  // UsesP("sth", v)
    std::string left_arg = left_ent.get_argument();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> right_entity_vec;
    right_entity_vec = synonym_to_entities_vec_.at(right_synonym);
    std::vector<std::string> right_var_vec;

    for (int i = 0; i < right_entity_vec.size(); i++) {
      auto *variable = dynamic_cast<Variable *>(right_entity_vec.at(i));
      if (variable != nullptr && VariableForwarder(get_reverse_, variable)->count(left_arg)) {
        right_var_vec.push_back(variable->get_name());
      }
    }
    ret->AddSingleColumn(right_synonym, right_var_vec);
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::WildCard) {  // UsesP("sth", _)
    std::string left_arg = left_ent.get_argument();
    Procedure *proc = pkb_->get_procedure(left_arg);
    if (proc == nullptr || ProcedureForwarder(get_normal_, proc)->empty()) {
      // If procedure with left arg as name
      // does not use anything then return nullptr
      return nullptr;
    }
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::Argument) {  // UsesP("sth", "x")
    std::string left_arg = left_ent.get_argument();
    std::string right_arg = right_ent.get_argument();
    Procedure *proc = pkb_->get_procedure(left_arg);
    if (!ProcedureForwarder(get_normal_, proc)->count(right_arg)) {
      // Return nullptr if this relationship_ is false
      return nullptr;
    }
  }
  return ret;
}

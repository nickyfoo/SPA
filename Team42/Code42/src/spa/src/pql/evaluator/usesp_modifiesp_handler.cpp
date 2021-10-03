#include "usesp_modifiesp_handler.h"

UsesPModifiesPHandler::UsesPModifiesPHandler() = default;
UsesPModifiesPHandler *UsesPModifiesPHandler::instance_ = nullptr;

UsesPModifiesPHandler *UsesPModifiesPHandler::get_instance() {
  if (!instance_) {
    instance_ = new UsesPModifiesPHandler();
  }
  return instance_;
}

void UsesPModifiesPHandler::set_args(PKB *pkb,
                                     std::unordered_map<std::string, std::vector<Entity *>>
                                     *synonym_to_entity_result,
                                     SuchThatClause relationship,
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
  EntRef left_ent = relationship_.get_left_ref()->get_ent_ref();
  EntRef right_ent = relationship_.get_right_ref()->get_ent_ref();

  // Going through 6 different cases for UsesP
  if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::Synonym) {  // UsesP(p, v)
    std::string left_synonym = left_ent.get_synonym();
    std::string right_synonym = right_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    std::vector<Entity *> *right_entity_vec;
    right_entity_vec = &synonym_to_entity_result_->at(right_synonym);

    left_entity_vec->erase(std::remove_if(left_entity_vec->begin(),
                                          left_entity_vec->end(),
                                          [this, &right_entity_vec](Entity *entity) {
                                            auto *proc = dynamic_cast<Procedure *>(entity);
                                            bool has_matching_follower = false;
                                            std::set<std::string> *follower_set =
                                                ProcedureForwarder(get_normal_, proc);
                                            // Check if followers contain something
                                            // from right arg vector
                                            // remove that procedure if it doesn't
                                            for (std::string follower : *follower_set) {
                                              for (Entity *ent : *right_entity_vec) {
                                                std::string right_name =
                                                    dynamic_cast<Variable *>(ent)->get_name();
                                                if (right_name == follower) {
                                                  has_matching_follower = true;
                                                  break;
                                                }
                                              }
                                            }
                                            return proc == nullptr ||
                                                follower_set->empty() ||
                                                !has_matching_follower;
                                          }),
                           left_entity_vec->end());

    right_entity_vec->erase(std::remove_if(right_entity_vec->begin(),
                                           right_entity_vec->end(),
                                           [this, &left_entity_vec](Entity *entity) {
                                             auto *variable = dynamic_cast<Variable *>(entity);
                                             bool has_matching_followee = false;
                                             std::set<std::string> *followee_set =
                                                 VariableForwarder(get_reverse_, variable);
                                             // Check if followees contain something
                                             // from left arg vector
                                             // remove that variable if it doesn't
                                             for (std::string followee : *followee_set) {
                                               for (Entity *ent : *left_entity_vec) {
                                                 std::string left_name =
                                                     dynamic_cast<Procedure *>(ent)->get_name();
                                                 if (left_name == followee) {
                                                   has_matching_followee = true;
                                                   break;
                                                 }
                                               }
                                             }
                                             return variable == nullptr ||
                                                 followee_set->empty() ||
                                                 !has_matching_followee;
                                           }),
                            right_entity_vec->end());
  } else if (left_ent.get_type() == EntRefType::Synonym &&
      right_ent.get_type() == EntRefType::WildCard) {  // UsesP(p, _)
    std::string left_synonym = left_ent.get_synonym();
    std::vector<Entity *> *left_entity_vec;
    left_entity_vec = &synonym_to_entity_result_->at(left_synonym);
    for (int i = 0; i < left_entity_vec->size(); i++) {
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec->at(i));
      // Remove each procedure that doesnt use anything.
      if (proc == nullptr || ProcedureForwarder(get_normal_, proc)->empty()) {
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
      auto *proc = dynamic_cast<Procedure *>(left_entity_vec->at(i));
      // Remove each procedure that doesnt have right arg in its uses
      if (proc == nullptr || !ProcedureForwarder(get_normal_, proc)->count(right_arg)) {
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
      if (variable == nullptr || !VariableForwarder(get_reverse_, variable)->count(left_arg)) {
        right_entity_vec->erase(right_entity_vec->begin() + i);
        i--;
      }
    }
  } else if (left_ent.get_type() == EntRefType::Argument &&
      right_ent.get_type() == EntRefType::WildCard) {  // UsesP("sth", _)
    std::string left_arg = left_ent.get_argument();
    Procedure *proc = pkb_->get_procedure(left_arg);
    if (proc == nullptr || ProcedureForwarder(get_normal_, proc)->empty()) {
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

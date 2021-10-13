#include "calls_handler.h"

CallsHandler::CallsHandler() = default;
CallsHandler *CallsHandler::instance_ = nullptr;

CallsHandler *CallsHandler::get_instance() {
  if (!instance_) {
    instance_ = new CallsHandler();
  }
  return instance_;
}

void CallsHandler::set_args(
    PKB *pkb, std::shared_ptr<SuchThatClause> relationship,
    std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
}

ResultTable *CallsHandler::evaluate(std::set<std::string> (Procedure::*get_normal)(),
                                    std::set<std::string> (Procedure::*get_reverse)()) {
  ResultTable *ret = new ResultTable();
  EntRef left_ent = relationship_->get_left_ref()->get_ent_ref();
  EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();

  if (left_ent.get_type() == EntRefType::Synonym && left_ent.get_type() == EntRefType::Synonym) {
    // Calls(a, b)
  } else if (left_ent.get_type() == EntRefType::Synonym && right_ent.get_type() == EntRefType::Argument) {
    // Calls(a, "y")
  } else if (left_ent.get_type() == EntRefType::Synonym && right_ent.get_type() == EntRefType::WildCard) {
    // Calls(a, _)
  } else if (left_ent.get_type() == EntRefType::Argument && right_ent.get_type() == EntRefType::Synonym) {
    // Calls("x", b)
  } else if (left_ent.get_type() == EntRefType::Argument && right_ent.get_type() == EntRefType::Argument) {
    // Calls("x", "y")
  } else if (left_ent.get_type() == EntRefType::Argument && right_ent.get_type() == EntRefType::WildCard) {
    // Calls("x", _)
  } else if (left_ent.get_type() == EntRefType::WildCard && right_ent.get_type() == EntRefType::Synonym) {
    // Calls(_, b)
  } else if (left_ent.get_type() == EntRefType::WildCard && right_ent.get_type() == EntRefType::Argument) {
    // Calls(_, "y")
  } else if (left_ent.get_type() == EntRefType::WildCard && right_ent.get_type() == EntRefType::WildCard) {
    // Calls(_, _)
  }

  return ret;
}

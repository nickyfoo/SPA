#include <cassert>

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

ResultTable *CallsHandler::EvaluateCalls() {
  return Evaluate(&Procedure::get_calls, &Procedure::get_callers);
}

ResultTable *CallsHandler::EvaluateCallsT() {
  return Evaluate(&Procedure::get_calls_star, &Procedure::get_callers_star);
}

ResultTable *CallsHandler::Evaluate(std::set<std::string> *(Procedure::*get_normal)(),
                                    std::set<std::string> *(Procedure::*get_reverse)()) {
  ResultTable *ret = new ResultTable();
  EntRef left_ent = relationship_->get_left_ref()->get_ent_ref();
  EntRef right_ent = relationship_->get_right_ref()->get_ent_ref();

  if (left_ent.get_type() == EntRefType::Synonym && right_ent.get_type() == EntRefType::Synonym) {
    // 1. Calls(a, b)
    auto left_synonym = left_ent.get_synonym();
    auto right_synonym = right_ent.get_synonym();

    if (left_synonym == right_synonym) {
      // recursive calls not allowed
      return nullptr;
    } else {
      std::vector<std::string> proc_left_vec;
      std::vector<std::string> proc_right_vec;

      for (auto e_left : synonym_to_entities_vec_.at(left_synonym)) {
        for (auto e_right : synonym_to_entities_vec_.at(right_synonym)) {
          auto *proc_left = dynamic_cast<Procedure *>(e_left);
          auto *proc_right = dynamic_cast<Procedure *>(e_right);

          assert(proc_left != nullptr && proc_right != nullptr);

          auto proc_left_calls = (proc_left->*get_normal)();
          if (proc_left_calls->find(proc_right->get_name()) != proc_left_calls->end()) {
            proc_left_vec.push_back(proc_left->get_name());
            proc_right_vec.push_back(proc_right->get_name());
          }
        }
      }

      ret->AddDoubleColumns(left_synonym, proc_left_vec, right_synonym, proc_right_vec);
    }
  } else if (left_ent.get_type() == EntRefType::Synonym &&
             right_ent.get_type() == EntRefType::Argument) {
    // 2. Calls(a, "y")
    auto left_synonym = left_ent.get_synonym();
    std::vector<std::string> proc_vec;

    for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
      auto *proc_left = dynamic_cast<Procedure *>(e);

      assert(proc_left != nullptr);

      auto proc_left_calls = (proc_left->*get_normal)();
      if (proc_left_calls->find(right_ent.get_argument()) != proc_left_calls->end()) {
        proc_vec.push_back(proc_left->get_name());
      }
    }

    ret->AddSingleColumn(left_synonym, proc_vec);
  } else if (left_ent.get_type() == EntRefType::Synonym &&
             right_ent.get_type() == EntRefType::WildCard) {
    // 3. Calls(a, _)
    auto left_synonym = left_ent.get_synonym();
    std::vector<std::string> proc_vec;

    for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
      auto *proc_left = dynamic_cast<Procedure *>(e);

      assert(proc_left != nullptr);

      auto proc_left_calls = (proc_left->*get_normal)();
      if (!proc_left_calls->empty()) {
        proc_vec.push_back(proc_left->get_name());
      }
    }

    ret->AddSingleColumn(left_synonym, proc_vec);
  } else if (left_ent.get_type() == EntRefType::Argument &&
             right_ent.get_type() == EntRefType::Synonym) {
    // 4. Calls("x", b)
    auto left_proc_name = left_ent.get_argument();

    auto *proc_left = pkb_->get_procedure(left_proc_name);
    if (proc_left == nullptr) {
      return ret;
    }

    auto proc_left_calls = (proc_left->*get_normal)();

    std::vector<std::string> proc_vec;
    for (auto e : *proc_left_calls) {
      proc_vec.push_back(e);
    }

    auto right_synonym = right_ent.get_synonym();
    ret->AddSingleColumn(right_synonym, proc_vec);
  } else if (left_ent.get_type() == EntRefType::Argument &&
             right_ent.get_type() == EntRefType::Argument) {
    // 5. Calls("x", "y")
    auto left_proc_name = left_ent.get_argument();
    auto right_proc_name = right_ent.get_argument();

    auto *proc_left = pkb_->get_procedure(left_proc_name);
    if (proc_left == nullptr) {
      return ret;
    }

    auto proc_left_calls = (proc_left->*get_normal)();

    if (proc_left_calls->find(right_proc_name) == proc_left_calls->end()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == EntRefType::Argument &&
             right_ent.get_type() == EntRefType::WildCard) {
    // 6. Calls("x", _)
    auto left_proc_name = left_ent.get_argument();

    auto *proc_left = pkb_->get_procedure(left_proc_name);
    if (proc_left == nullptr) {
      return nullptr;
    }

    auto proc_left_calls = (proc_left->*get_normal)();

    if (proc_left_calls->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == EntRefType::WildCard &&
             right_ent.get_type() == EntRefType::Synonym) {
    // 7. Calls(_, b)
    auto right_synonym = right_ent.get_synonym();

    std::vector<std::string> proc_vec;

    for (auto e : synonym_to_entities_vec_.at(right_synonym)) {
      auto *proc_right = dynamic_cast<Procedure *>(e);

      assert(proc_right != nullptr);

      auto proc_right_callers = (proc_right->*get_reverse)();
      if (!proc_right_callers->empty()) {
        proc_vec.push_back(proc_right->get_name());
      }
    }

    ret->AddSingleColumn(right_synonym, proc_vec);
  } else if (left_ent.get_type() == EntRefType::WildCard &&
             right_ent.get_type() == EntRefType::Argument) {
    // 8. Calls(_, "y")
    auto right_proc_name = right_ent.get_argument();
    auto *proc_right = pkb_->get_procedure(right_proc_name);

    if (proc_right == nullptr) {
      return nullptr;
    }

    auto proc_right_callers = (proc_right->*get_reverse)();
    if (proc_right_callers->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == EntRefType::WildCard &&
             right_ent.get_type() == EntRefType::WildCard) {
    // 9. Calls(_, _)
    auto procs = pkb_->get_all_procedures();
    for (auto p : procs) {
      if (!(p->*get_normal)()->empty()) {
        return ret;
      }
    }

    return nullptr;
  }

  return ret;
}

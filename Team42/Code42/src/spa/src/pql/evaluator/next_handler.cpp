#include <cassert>

#include "next_handler.h"

NextHandler::NextHandler() {}

NextHandler *NextHandler::instance_ = nullptr;

NextHandler *NextHandler::get_instance() {
  if (!instance_) {
    instance_ = new NextHandler();
  }
  return instance_;
}

void NextHandler::set_args(
    PKB *pkb, std::shared_ptr<SuchThatClause> relationship,
    std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
}

ResultTable *NextHandler::EvaluateNext() { return Evaluate(&PKB::get_next); }

ResultTable *NextHandler::EvaluateNextT() { return Evaluate(&PKB::get_next_star); }

ResultTable *NextHandler::Evaluate(std::set<std::pair<int, int>> (PKB::*func)(int, int)) {
  ResultTable *ret = new ResultTable();

  relationship_->get_left_ref()->get_type();

  LineRef left_ent = relationship_->get_left_ref()->get_line_ref();
  LineRef right_ent = relationship_->get_right_ref()->get_line_ref();

  if (left_ent.get_type() == LineRefType::LineNum && right_ent.get_type() == LineRefType::LineNum) {
    // 1. Next(1, 2)
    int left_arg = left_ent.get_line_num();
    int right_arg = right_ent.get_line_num();

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res.empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == LineRefType::Synonym &&
             right_ent.get_type() == LineRefType::LineNum) {
    // 2. Next(a, 2)
    auto right_arg = right_ent.get_line_num();
    std::vector<std::string> line_vec;

    auto left_synonym = left_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto left_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg).empty()) {
        line_vec.push_back(std::to_string(left_arg));
      }
    }

    ret->AddSingleColumn(left_synonym, line_vec);
  } else if (left_ent.get_type() == LineRefType::LineNum &&
             right_ent.get_type() == LineRefType::Synonym) {
    // 3. Next(1, b)
    auto left_arg = left_ent.get_line_num();
    std::vector<std::string> line_vec;

    auto right_synonym = right_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(right_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto right_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg).empty()) {
        line_vec.push_back(std::to_string(right_arg));
      }
    }

    ret->AddSingleColumn(right_synonym, line_vec);
  } else if (left_ent.get_type() == LineRefType::Synonym &&
             right_ent.get_type() == LineRefType::Synonym) {
    // 4. Next(a, b)
    auto left_synonym = left_ent.get_synonym();
    auto right_synonym = right_ent.get_synonym();

    if (left_synonym == right_synonym) {
      std::vector<std::string> line_vec;
      for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
        auto *stmt = dynamic_cast<Statement *>(e);
        assert(stmt != nullptr);
        auto arg = stmt->get_stmt_no();

        if (!(pkb_->*func)(arg, arg).empty()) {
          line_vec.push_back(std::to_string(arg));
        }
      }

      ret->AddSingleColumn(left_synonym, line_vec);
    } else {
      std::vector<std::string> left_line_vec;
      std::vector<std::string> right_line_vec;

      for (auto e_left : synonym_to_entities_vec_.at(left_synonym)) {
        for (auto e_right : synonym_to_entities_vec_.at(right_synonym)) {
          auto *stmt_left = dynamic_cast<Statement *>(e_left);
          auto *stmt_right = dynamic_cast<Statement *>(e_right);

          assert(stmt_left != nullptr);
          assert(stmt_right != nullptr);

          auto left_arg = stmt_left->get_stmt_no();
          auto right_arg = stmt_right->get_stmt_no();

          if (!(pkb_->*func)(left_arg, right_arg).empty()) {
            left_line_vec.push_back(std::to_string(left_arg));
            right_line_vec.push_back(std::to_string(right_arg));
          }
        }
      }

      ret->AddDoubleColumns(left_synonym, left_line_vec, right_synonym, right_line_vec);
    }
  } else if (left_ent.get_type() == LineRefType::WildCard &&
             right_ent.get_type() == LineRefType::WildCard) {
    // 5. Next(_, _)
    int left_arg = PKB::kWild;
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res.empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == LineRefType::LineNum &&
             right_ent.get_type() == LineRefType::WildCard) {
    // 6. Next(1, _)
    int left_arg = left_ent.get_line_num();
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res.empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == LineRefType::WildCard &&
             right_ent.get_type() == LineRefType::LineNum) {
    // 7. Next(_, 1)
    int left_arg = PKB::kWild;
    int right_arg = right_ent.get_line_num();

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res.empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == LineRefType::Synonym &&
             right_ent.get_type() == LineRefType::WildCard) {
    // 8. Next(a, _)
    int right_arg = PKB::kWild;
    std::vector<std::string> line_vec;

    auto left_synonym = left_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto left_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg).empty()) {
        line_vec.push_back(std::to_string(left_arg));
      }
    }

    ret->AddSingleColumn(left_synonym, line_vec);
  } else if (left_ent.get_type() == LineRefType::WildCard &&
             right_ent.get_type() == LineRefType::Synonym) {
    // 9. Next(_, a)
    int left_arg = PKB::kWild;
    std::vector<std::string> line_vec;

    auto right_synonym = right_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(right_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto right_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg).empty()) {
        line_vec.push_back(std::to_string(right_arg));
      }
    }

    ret->AddSingleColumn(right_synonym, line_vec);
  }

  return ret;
}

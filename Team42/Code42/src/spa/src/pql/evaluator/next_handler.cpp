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
    PKB *pkb, std::shared_ptr<SuchThatClause> relationship) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
}

ResultTable *NextHandler::EvaluateNext() {
  return evaluate(&PKB::get_next);
}

ResultTable *NextHandler::EvaluateNextT() {
  return evaluate(&PKB::get_next_star);
}

ResultTable *NextHandler::evaluate(std::set<std::pair<int, int>> (PKB::*func)(int, int)) {
  ResultTable *ret = new ResultTable();

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
    int left_arg = PKB::kWild;
    int right_arg = right_ent.get_line_num();

    auto res = (pkb_->*func)(left_arg, right_arg);

    std::vector<std::string> line_vec;
    for (auto p : res) {
      line_vec.push_back(std::to_string(p.first));
    }

    auto left_synonym = left_ent.get_synonym();
    ret->AddSingleColumn(left_synonym, line_vec);
  } else if (left_ent.get_type() == LineRefType::LineNum &&
             right_ent.get_type() == LineRefType::Synonym) {
    // 3. Next(1, b)
    int left_arg = left_ent.get_line_num();
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);

    std::vector<std::string> line_vec;
    for (auto p : res) {
      line_vec.push_back(std::to_string(p.second));
    }

    auto right_synonym = right_ent.get_synonym();
    ret->AddSingleColumn(right_synonym, line_vec);
  } else if (left_ent.get_type() == LineRefType::Synonym &&
             right_ent.get_type() == LineRefType::Synonym) {
    // 4. Next(a, b)
    int left_arg = PKB::kWild;
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);

    std::vector<std::string> left_line_vec;
    std::vector<std::string> right_line_vec;
    for (auto p : res) {
      left_line_vec.push_back(std::to_string(p.first));
      right_line_vec.push_back(std::to_string(p.second));
    }

    auto left_synonym = left_ent.get_synonym();
    auto right_synonym = right_ent.get_synonym();
    ret->AddDoubleColumns(left_synonym, left_line_vec, right_synonym, right_line_vec);
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
    int left_arg = PKB::kWild;
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);

    std::vector<std::string> line_vec;
    for (auto p : res) {
      line_vec.push_back(std::to_string(p.first));
    }

    auto left_synonym = left_ent.get_synonym();
    ret->AddSingleColumn(left_synonym, line_vec);
  } else if (left_ent.get_type() == LineRefType::WildCard &&
             right_ent.get_type() == LineRefType::Synonym) {
    // 9. Next(_, a)
    int left_arg = PKB::kWild;
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);

    std::vector<std::string> line_vec;
    for (auto p : res) {
      line_vec.push_back(std::to_string(p.second));
    }

    auto right_synonym = right_ent.get_synonym();
    ret->AddSingleColumn(right_synonym, line_vec);
  }

  return ret;
}

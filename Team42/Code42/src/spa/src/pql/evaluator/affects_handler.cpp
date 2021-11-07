#include "affects_handler.h"
#include "ast.h"
#include <assert.h>

AffectsHandler::AffectsHandler() {}

AffectsHandler *AffectsHandler::instance_ = nullptr;

AffectsHandler *AffectsHandler::get_instance() {
  if (!instance_) {
    instance_ = new AffectsHandler();
  }
  return instance_;
}

void AffectsHandler::set_args(
    PKB *pkb, std::shared_ptr<SuchThatClause> relationship,
    std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec) {
  this->pkb_ = pkb;
  this->relationship_ = relationship;
  this->synonym_to_entities_vec_ = synonym_to_entities_vec;
}

ResultTable *AffectsHandler::EvaluateAffects() { return Evaluate(&PKB::get_affects); }

ResultTable *AffectsHandler::EvaluateAffectsT() { return Evaluate(&PKB::get_affects_star); }

ResultTable *AffectsHandler::EvaluateAffectsBip() { return Evaluate(&PKB::get_affects_bip); }

ResultTable *AffectsHandler::EvaluateAffectsTBip() { return Evaluate(&PKB::get_affects_bip_star); }

// if is nullpointer, means False
// if is empty, means that True with no synonyms
ResultTable *AffectsHandler::Evaluate(std::set<std::pair<int, int>> *(PKB::*func)(int, int)) {
  ResultTable *ret = new ResultTable();

  relationship_->get_left_ref()->get_type();

  StmtRef left_ent = relationship_->get_left_ref()->get_stmt_ref();
  StmtRef right_ent = relationship_->get_right_ref()->get_stmt_ref();
  if (left_ent.get_type() == StmtRefType::StmtNum && right_ent.get_type() == StmtRefType::StmtNum) {
    // 1. Affects(1, 2)
    int left_arg = left_ent.get_stmt_num();
    int right_arg = right_ent.get_stmt_num();

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::StmtNum) {
    // 2. Affects(a, 2)
    auto right_arg = right_ent.get_stmt_num();
    std::vector<std::string> stmt_vec;

    auto left_synonym = left_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto left_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg)->empty()) {
        stmt_vec.push_back(std::to_string(left_arg));
      }
    }

    ret->AddSingleColumn(left_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::Synonym) {
    // 3. Affects(1, b)
    auto left_arg = left_ent.get_stmt_num();
    std::vector<std::string> stmt_vec;

    auto right_synonym = right_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(right_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto right_arg = stmt->get_stmt_no();

      if (!(pkb_->*func)(left_arg, right_arg)->empty()) {
        stmt_vec.push_back(std::to_string(right_arg));
      }
    }

    ret->AddSingleColumn(right_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::Synonym) {
    // 4. Affects(a, b)
    auto left_synonym = left_ent.get_synonym();
    auto right_synonym = right_ent.get_synonym();

    if (left_synonym == right_synonym) {
      std::vector<std::string> stmt_vec;
      for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
        auto *stmt = dynamic_cast<Statement *>(e);
        assert(stmt != nullptr);
        auto arg = stmt->get_stmt_no();

        if (!(pkb_->*func)(arg, arg)->empty()) {
          stmt_vec.push_back(std::to_string(arg));
        }
      }

      ret->AddSingleColumn(left_synonym, stmt_vec);
    } else {
      std::vector<std::string> left_stmt_vec;
      std::vector<std::string> right_stmt_vec;

      for (auto e_left : synonym_to_entities_vec_.at(left_synonym)) {
        for (auto e_right : synonym_to_entities_vec_.at(right_synonym)) {
          auto *stmt_left = dynamic_cast<Statement *>(e_left);
          auto *stmt_right = dynamic_cast<Statement *>(e_right);

          assert(stmt_left != nullptr);
          assert(stmt_right != nullptr);

          auto left_arg = stmt_left->get_stmt_no();
          auto right_arg = stmt_right->get_stmt_no();

          if (!(pkb_->*func)(left_arg, right_arg)->empty()) {
            left_stmt_vec.push_back(std::to_string(left_arg));
            right_stmt_vec.push_back(std::to_string(right_arg));
          }
        }
      }

      ret->AddDoubleColumns(left_synonym,
                            left_stmt_vec,
                            right_synonym,
                            right_stmt_vec);
    }
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::WildCard) {
    // 5. Affects(_, _)
    int left_arg = PKB::kWild;
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::StmtNum &&
      right_ent.get_type() == StmtRefType::WildCard) {
    // 6. Affects(1, _)
    int left_arg = left_ent.get_stmt_num();
    int right_arg = PKB::kWild;

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::StmtNum) {
    // 7. Affects(_, 1)
    int left_arg = PKB::kWild;
    int right_arg = right_ent.get_stmt_num();

    auto res = (pkb_->*func)(left_arg, right_arg);
    if (res->empty()) {
      return nullptr;
    }
  } else if (left_ent.get_type() == StmtRefType::Synonym &&
      right_ent.get_type() == StmtRefType::WildCard) {
    // 8. Affects(a, _)
    int right_arg = PKB::kWild;
    std::vector<std::string> stmt_vec;

    auto left_synonym = left_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(left_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto left_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg)->empty()) {
        stmt_vec.push_back(std::to_string(left_arg));
      }
    }

    ret->AddSingleColumn(left_synonym, stmt_vec);
  } else if (left_ent.get_type() == StmtRefType::WildCard &&
      right_ent.get_type() == StmtRefType::Synonym) {
    // 9. Affects(_, a)
    int left_arg = PKB::kWild;
    std::vector<std::string> stmt_vec;

    auto right_synonym = right_ent.get_synonym();
    for (auto e : synonym_to_entities_vec_.at(right_synonym)) {
      auto *stmt = dynamic_cast<Statement *>(e);
      assert(stmt != nullptr);
      auto right_arg = stmt->get_stmt_no();
      if (!(pkb_->*func)(left_arg, right_arg)->empty()) {
        stmt_vec.push_back(std::to_string(right_arg));
      }
    }

    ret->AddSingleColumn(right_synonym, stmt_vec);
  }

  return ret;
}


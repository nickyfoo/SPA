#include "stmt_ref.h"

StmtRef::StmtRef() {
    this->type_ = StmtRefType::None;
    this->stmt_num_ = -1;
}

void StmtRef::set_synonym(const std::string& syn) {
    this->type_ = StmtRefType::Synonym;
    this->synonym_ = syn;
}

void StmtRef::set_stmt_num(int line_num) {
    this->type_ = StmtRefType::StmtNum;
    this->stmt_num_ = line_num;
}

void StmtRef::set_wild_card() {
    this->type_ = StmtRefType::WildCard;
}

StmtRefType StmtRef::get_type() {
    return this->type_;
}

std::string StmtRef::get_synonym() {
    return this->synonym_;
}

int StmtRef::get_stmt_num() const {
    return this->stmt_num_;
}

// used for logging purposes
std::string StmtRef::get_value() {
    switch (this->type_) {
      case StmtRefType::Synonym:
        return this->synonym_;
      case StmtRefType::StmtNum:
        return std::to_string(this->stmt_num_);
      case StmtRefType::WildCard:
        return "_";
      default:
        return "No value!";
    }
}

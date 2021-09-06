#include "StmtRef.h"

StmtRef::StmtRef() {
    this->type = StmtRefType::None;
    this->stmt_num = -1;
}

void StmtRef::setSynonym(std::string& synonym) {
    this->type = StmtRefType::Synonym;
    this->synonym = synonym;
}

void StmtRef::setStmtNum(int stmt_num) {
    this->type = StmtRefType::StmtNum;
    this->stmt_num = stmt_num;
}

void StmtRef::setWildCard() {
    this->type = StmtRefType::WildCard;
}

StmtRefType StmtRef::getType() {
    return this->type;
}

std::string StmtRef::getSynonym() {
    return this->synonym;
}

int StmtRef::getStmtNum() {
    return this->stmt_num;
}

// used for logging purposes
std::string StmtRef::getValue() {
    switch (this->type) {
        case StmtRefType::Synonym:
            return this->synonym;
        case StmtRefType::StmtNum:
            return std::to_string(this->stmt_num);
        case StmtRefType::WildCard:
            return "_";
        default:
            return "No value!";
    }
}
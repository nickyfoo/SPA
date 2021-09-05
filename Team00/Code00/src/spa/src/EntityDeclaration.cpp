#include "EntityDeclaration.h"

EntityDeclaration::EntityDeclaration(EntityType type, std::string synonym) {
    this->type = type;
    this->synonym = synonym;
    this->stmt_num = -1;
    this->value = "";
}

EntityDeclaration::EntityDeclaration(std::string value) {
    std::string::const_iterator it = value.begin();
    while (it != value.end() && std::isdigit(*it)) ++it;
    if(!value.empty() && it == value.end()) {
        this->stmt_num = stoi(value);
        this->value = "";
    } else {
        this->value = value;
        this->stmt_num = -1;
    }
    this->type = EntityType::Argument;
    this->synonym = "";
}

std::string EntityDeclaration::getSynonym() {
    return this->synonym;
}

int EntityDeclaration::getStmtRef() {
    return this->stmt_num;
}

std::string EntityDeclaration::getEntRef() {
    return this->value;
}

EntityType EntityDeclaration::getType() {
    return this->type;
}

std::string EntityDeclaration::getValue() {
    if (isSynonym()) {
        return this->synonym;
    } else if (isValue()) {
        return this->value;
    } else if (isStmtNum()) {
        return std::to_string(this->stmt_num);
    } else {
        return "";
    }
}

bool EntityDeclaration::isSynonym() {
    return this->synonym != "";
}

bool EntityDeclaration::isStmtNum() {
    return this->stmt_num != -1;
}

bool EntityDeclaration::isValue() {
    return this->value != "";
}

bool EntityDeclaration::isWildCard() {
    return this->value == "_";
}
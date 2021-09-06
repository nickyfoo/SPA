#include "EntRef.h"

EntRef::EntRef() {
    this->type = EntRefType::None;
}

void EntRef::setSynonym(std::string& synonym) {
    this->type = EntRefType::Synonym;
    this->synonym = synonym;
}

void EntRef::setArgument(std::string& argument) {
    this->type = EntRefType::Argument;
    this->argument = argument;
}

void EntRef::setWildCard() {
    this->type = EntRefType::WildCard;
}
EntRefType EntRef::getType() {
    return this->type;
}

std::string EntRef::getSynonym() {
    return this->synonym;
}

std::string EntRef::getArgument() {
    return this->argument;
}

// used for logging purposes
std::string EntRef::getValue() {
    switch (this->type) {
        case EntRefType::Synonym:
            return this->synonym;
        case EntRefType::Argument:
            return this->argument;
        case EntRefType::WildCard:
            return "_";
        default:
            return "No value!";
    }
}
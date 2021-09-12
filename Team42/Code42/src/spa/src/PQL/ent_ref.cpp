#include "ent_ref.h"

EntRef::EntRef() {
    this->type_ = EntRefType::None;
}

void EntRef::set_synonym(const std::string &syn) {
    this->type_ = EntRefType::Synonym;
    this->synonym_ = syn;
}

void EntRef::set_argument(const std::string &arg) {
    this->type_ = EntRefType::Argument;
    this->argument_ = arg;
}

void EntRef::set_wild_card() {
    this->type_ = EntRefType::WildCard;
}

EntRefType EntRef::get_type() {
    return this->type_;
}

std::string EntRef::get_synonym() {
    return this->synonym_;
}

std::string EntRef::get_argument() {
    return this->argument_;
}

// used for logging purposes
std::string EntRef::get_value() {
    switch (this->type_) {
      case EntRefType::Synonym:
        return this->synonym_;
      case EntRefType::Argument:
        return this->argument_;
      case EntRefType::WildCard:
        return "_";
      default:
        return "No value!";
    }
}

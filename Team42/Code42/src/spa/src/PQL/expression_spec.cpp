#include "expression_spec.h"

ExpressionSpec::ExpressionSpec() {
    this->partial_pattern_ = false;
    this->wild_card_ = false;
}

std::string ExpressionSpec::get_expression() {
    return this->expression_;
}

void ExpressionSpec::set_expression(const std::string& str) {
    this->expression_ = str;
}

void ExpressionSpec::set_partial_pattern(bool is_partial_pattern) {
    this->partial_pattern_ = is_partial_pattern;
}

void ExpressionSpec::set_wild_card() {
    this->wild_card_ = true;
}

bool ExpressionSpec::IsPartialPattern() const {
    return this->partial_pattern_;
}

bool ExpressionSpec::IsWildCard() const {
    return this->wild_card_;
}

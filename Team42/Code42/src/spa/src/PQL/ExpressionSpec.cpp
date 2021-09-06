#include "ExpressionSpec.h"

ExpressionSpec::ExpressionSpec() {
    this->partial_pattern = false;
    this->wild_card = false;
}

std::string ExpressionSpec::getExpression() {
    return this->expression;
}

void ExpressionSpec::setExpression(std::string& str) {
    this->expression = str;
}

void ExpressionSpec::setPartialPattern(bool is_partial_pattern) {
    this->partial_pattern = is_partial_pattern;
}

void ExpressionSpec::setWildCard() {
    this->wild_card = true;
}

bool ExpressionSpec::isPartialPattern() const {
    return this->partial_pattern;
}

bool ExpressionSpec::isWildCard() const {
    return this->wild_card;
}
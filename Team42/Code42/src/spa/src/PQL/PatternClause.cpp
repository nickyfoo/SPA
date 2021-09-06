#include "PatternClause.h"

PatternClause::PatternClause(EntityDeclaration* synonym) {
    this->synonym = synonym;
    this->left_ref = nullptr;
    this->right_ref = nullptr;
}

bool PatternClause::setRef(EntRef* left, std::string& right) {
    this->left_ref = left;
    return isValidRightRef(right);
}

EntityDeclaration* PatternClause::getSynonym() {
    return this->synonym;
}

EntRef* PatternClause::getLeftRef() {
    return this->left_ref;
}

ExpressionSpec* PatternClause::getRightRef() {
    return this->right_ref;
}

bool PatternClause::isValidRightRef(std::string ref) {
    bool partial_pattern;
    auto* expSpec = new ExpressionSpec();
    if (ref == "_") {
        expSpec->setWildCard();
        this->right_ref = expSpec;
        return true;
    }

    if (ref.at(0) == '_' && ref.at(ref.length() - 1) == '_') {
        partial_pattern = true;
        ref = ref.substr(1, ref.length() - 2); // remove _
    } else {
        partial_pattern = false;
    }

    if (!((ref.at(0) == '\'' && ref.at(ref.length() - 1) == '\'') ||
          (ref.at(0) == '\"' && ref.at(ref.length() - 1) == '\"'))) { // check for '\''
        return false;
    }

    ref = ref.substr(1, ref.length() - 2); // remove ""
    bool expectingExp = false;
    for (char& c : ref) {
        if (!expectingExp && isExp(c)) {
            return false;
        } else {
            if (isExp(c)) {
                expectingExp = false;
            } else if (isChar(c)) {
                expectingExp = true;
            } else {
                return false;
            }
        }
    }

    if(!expectingExp && !ref.empty()) {
        return false;
    }

    expSpec->setExpression(ref);
    expSpec->setPartialPattern(partial_pattern);

    this->right_ref = expSpec;
    return true;
}

bool PatternClause::isChar(char c) {
    if (!((c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    || (c >= '0' && c <= '9')
    || c == '_'))
        return false;
    return true;
}

bool PatternClause::isExp(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/')
        return true;
    return false;
}
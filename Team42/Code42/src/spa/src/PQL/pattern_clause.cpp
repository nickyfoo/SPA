#include "pattern_clause.h"

PatternClause::PatternClause(EntityDeclaration *synonym) {
    this->synonym_ = synonym;
    this->left_ref_ = nullptr;
    this->right_ref_ = nullptr;
}

bool PatternClause::set_ref(EntRef *left, const std::string &right) {
    this->left_ref_ = left;
    return IsValidRightRef(right);
}

EntityDeclaration *PatternClause::get_synonym() {
    return this->synonym_;
}

EntRef *PatternClause::get_left_ref() {
    return this->left_ref_;
}

ExpressionSpec *PatternClause::get_right_ref() {
    return this->right_ref_;
}

bool PatternClause::IsValidRightRef(std::string ref) {
    bool partial_pattern;
    auto* expSpec = new ExpressionSpec();
    if (ref == "_") {
        expSpec->set_wild_card();
        this->right_ref_ = expSpec;
        return true;
    }

    if (ref.at(0) == '_' && ref.at(ref.length() - 1) == '_') {
        partial_pattern = true;
        ref = ref.substr(1, ref.length() - 2);  // remove _
    } else {
        partial_pattern = false;
    }

    // check for '\''
    if (!((ref.at(0) == '\'' && ref.at(ref.length() - 1) == '\'') ||
          (ref.at(0) == '\"' && ref.at(ref.length() - 1) == '\"'))) {
        return false;
    }

    ref = ref.substr(1, ref.length() - 2);  // remove ""
    bool expectingExp = false;
    for (char& c : ref) {
        if (!expectingExp && IsExp(c)) {
            return false;
        } else {
            if (IsExp(c)) {
                expectingExp = false;
            } else if (IsChar(c)) {
                expectingExp = true;
            } else {
                return false;
            }
        }
    }

    if (!expectingExp && !ref.empty()) {
        return false;
    }

    expSpec->set_expression(ref);
    expSpec->set_partial_pattern(partial_pattern);

    this->right_ref_ = expSpec;
    return true;
}

bool PatternClause::IsChar(char c) {
    if (!((c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    || (c >= '0' && c <= '9')
    || c == '_'))
        return false;
    return true;
}

bool PatternClause::IsExp(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/')
        return true;
    return false;
}

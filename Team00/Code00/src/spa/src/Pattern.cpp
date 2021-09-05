#include "Pattern.h"

Pattern::Pattern(std::string& synonym)
{
    this->synonym = synonym;
    this->left_ref = nullptr;
    this->right_ref = "";
    this->partial_pattern = false;
}

bool Pattern::setRef(Entity* left_ref, std::string& right_ref) {
    this->left_ref = left_ref;
    return isValidRightRef(right_ref);
}

std::string Pattern::getSynonym() {
    return this->synonym;
}

Entity* Pattern::getLeftRef() {
    return this->left_ref;
}

std::string Pattern::getRightRef() {
    return this->right_ref;
}

bool Pattern::isPartialPattern() const {
    return this->partial_pattern;
}

bool Pattern::isValidRightRef(std::string right_ref) {
    if (right_ref == "_") {
        this->right_ref = right_ref;
        return true;
    }

    if (right_ref.at(0) == '_' && right_ref.at(right_ref.length() - 1) == '_') {
        this->partial_pattern = true;
        right_ref = right_ref.substr(1, right_ref.length() - 2); // remove _
    } else {
        this->partial_pattern = false;
    }

    if (!((right_ref.at(0) == '\'' && right_ref.at(right_ref.length() - 1) == '\'') ||
        (right_ref.at(0) == '\"' && right_ref.at(right_ref.length() - 1) == '\"'))) { // check for '\''
        return false;
    }

    right_ref = right_ref.substr(1, right_ref.length() - 2); // remove ""
    bool expectingExp = false;
    for (char& c : right_ref) {
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
    this->right_ref = right_ref;
    return true;
}

bool Pattern::isChar(char c) {
    if (!((c >= 'a' && c <= 'z')
    || (c >= 'A' && c <= 'Z')
    || (c >= '0' && c <= '9')
    || c == '_'))
        return false;
    return true;
}

bool Pattern::isExp(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/')
        return true;
    return false;
}
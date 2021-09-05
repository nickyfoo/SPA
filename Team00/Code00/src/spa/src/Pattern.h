#pragma once
#include <string>
#include "Entity.h"

class Pattern {
public:
    Pattern(std::string synonym);
    bool setRef(Entity* left_ref, string right_ref);
    std::string getSynonym();
    Entity* getLeftRef();
    string getRightRef();
    bool isPartialPattern();
private:
    string synonym;
    Entity* left_ref;
    string right_ref;
    bool partial_pattern;
    bool isValidRightRef(string right_ref);
    bool isChar(char c);
    bool isExp(char c);
};
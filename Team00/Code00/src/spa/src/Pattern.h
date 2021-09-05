#pragma once
#include <string>
#include "Entity.h"

class Pattern {
public:
    explicit Pattern(std::string& synonym);
    bool setRef(Entity* left_ref, std::string& right_ref);
    std::string getSynonym();
    Entity* getLeftRef();
    std::string getRightRef();
    bool isPartialPattern() const;
private:
    std::string synonym;
    Entity* left_ref;
    std::string right_ref;
    bool partial_pattern;
    bool isValidRightRef(std::string right_ref);
    static bool isChar(char c);
    static bool isExp(char c);
};
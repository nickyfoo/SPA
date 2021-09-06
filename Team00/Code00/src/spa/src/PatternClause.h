#pragma once
#include <string>
#include "EntityDeclaration.h"
#include "ExpressionSpec.h"
#include "EntRef.h"

class PatternClause {
public:
    explicit PatternClause(EntityDeclaration* synonym);
    bool setRef(EntRef* left, std::string& right);
    EntityDeclaration* getSynonym();
    EntRef* getLeftRef();
    ExpressionSpec* getRightRef();

private:
    EntityDeclaration* synonym;
    EntRef* left_ref;
    ExpressionSpec* right_ref;
    bool isValidRightRef(std::string ref);
    static bool isChar(char c);
    static bool isExp(char c);
};
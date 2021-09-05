#pragma once

#include <stdio.h>
#include <unordered_map>
#include <string>
#include "Entities/Statement.h"

class PatternManager {
public:
    static bool checkAssignmentRhs(Statement* assignmentStmt, std::string pattern, bool isPartialMatch);

private:
    inline static const std::unordered_map<char, int> opsMap = {{'(', 1}, {')', 1}, {'*', 2}, {'/', 2}, {'+', 3}, {'-', 3}};
    static std::string getPostfixExpr(std::string infixExpr);
};
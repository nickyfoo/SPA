#pragma once

#include <cstdio>
#include <unordered_map>
#include <string>
#include "entities/statement.h"

class PatternManager {
 public:
  PatternManager();

  ~PatternManager();

  // Tests the assignment expression against the given pattern.
  // Returns true if pattern matches.
  bool TestAssignmentPattern(Statement *assignment_stmt, const std::string &pattern, bool is_partial_match);
  // Tests for a variable in the expression of the if statement.
  // Returns true if variable matches.
  bool TestIfWhilePattern(Statement *stmt, const std::string &variable);

 private:
  // Map of operands to its order of precedence.
  inline static const std::unordered_map<char, int>
      kOpsMap = {{'(', 1}, {')', 1},
                 {'*', 2}, {'/', 2}, {'%', 2},
                 {'+', 3}, {'-', 3}};

  // Gets the postfix expression from an infix expression string.
  std::string GetPostfixExpr(std::string infix_expr);
};
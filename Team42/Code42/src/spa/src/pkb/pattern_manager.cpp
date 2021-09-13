#include "pattern_manager.h"

#include <iostream>
#include <unordered_map>
#include <stack>
#include <sstream>

auto CheckNextChar(bool is_ident);

// Returns a curried function that checks for validity of the next character
// depending on the type of variable.
auto CheckNextChar(bool is_ident) {
  return [=](char c) {
    return is_ident ? std::isalnum(c) : std::isdigit(c);
  };
}

// Returns an infix expression in postfix notation
std::string PatternManager::GetPostfixExpr(std::string infix_expr) {
  std::stack<char> ops;
  std::stringstream output;
  std::string::iterator it;
  try {
    for (it = infix_expr.begin(); it != infix_expr.end(); ++it) {
      if (std::isalnum(*it)) {
        // Flag for whether variable is an identifier or a constant
        bool is_ident = std::isalpha(*it);
        output << *it;

        // Advance iterator if next character is valid
        while (std::next(it, 1) != infix_expr.end() &&
            CheckNextChar(is_ident)(*std::next(it, 1))) {
          std::advance(it, 1);
          output << *it;
        }
        output << ' ';
        continue;
      }

      switch (*it) {
        case '(':ops.push(*it);
          break;
        case ')':
          while (ops.top() != '(' && !ops.empty()) {
            output << ops.top() << ' ';
            ops.pop();
          }
          break;
        case '+':
        case '-':
        case '*':
        case '/': {
          if (ops.empty() || ops.top() == '(') {
            ops.push(*it);
            break;
          }

          auto last_ops = kOpsMap.find(ops.top());
          auto new_ops = kOpsMap.find(*it);
          if (new_ops->second < last_ops->second) {
            // Case 1: New operand has a higher precedence than the top of the stack
            ops.push(*it);
          } else if (new_ops->second > last_ops->second) {
            // Case 2: New operand has a lower precedence than the top of the stack
            while (new_ops->second >= last_ops->second && !ops.empty()) {
              // Pop stack while precedence of the new operand is lte the top of the stack
              output << ops.top() << ' ';
              ops.pop();
              if (ops.empty()) break;
              last_ops = kOpsMap.find(ops.top());
            }
            ops.push(*it);
          } else {
            // Case 3: New operand has a same precedence as the top of the stack
            // Pop the ops, add it to the output and push the new operand
            output << ops.top() << ' ';
            ops.pop();
            ops.push(*it);
          }
          break;
        }
        case ' ': break;
        default: throw std::invalid_argument("Expression is not valid");
      }
    }

    while (!ops.empty()) {
      if (ops.top() == '(' || ops.top() == ')') {
        ops.pop();
        continue;
      }
      output << ops.top() << ' ';
      ops.pop();
    }
  } catch (std::invalid_argument e) {
    std::cerr << e.what() << std::endl;
  }
  return output.str();
}

bool PatternManager::TestAssignmentPattern(Statement *assignment_stmt, std::string pattern, bool is_partial_match) {
  std::string postfix_pattern = GetPostfixExpr(pattern);
  postfix_pattern.pop_back();
  std::string assignment_expr = assignment_stmt->get_expr_string();
  if (is_partial_match) {
    return assignment_expr.find(postfix_pattern) != std::string::npos;
  } else {
    return assignment_expr == postfix_pattern;
  }
}

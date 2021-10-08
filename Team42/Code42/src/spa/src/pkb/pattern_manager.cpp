#include "pattern_manager.h"

#include <iostream>
#include <stack>
#include <sstream>

PatternManager::PatternManager() = default;

PatternManager::~PatternManager() = default;

auto CheckNextChar(bool is_ident);

// Returns a curried function that checks for validity of the next character
// depending on the type of variable.
auto CheckNextChar(bool is_ident) {
  return [=](char c) {
    return is_ident
           ? std::isalpha(c) || std::isdigit(c)
           : std::isdigit(c);
  };
}

// Returns an infix expression in postfix notation
std::string PatternManager::GetPostfixExpr(std::string infix_expr) {
  std::stack<char> ops;
  std::stringstream output;
  std::string::iterator it;
  try {
    output << ' ';
    for (it = infix_expr.begin(); it != infix_expr.end(); ++it) {
      if (std::isalpha(*it) || std::isdigit(*it)) {
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
          ops.pop();
          break;
        case '+':
        case '-':
        case '*':
        case '/':
        case '%': {
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
        case '\t':
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
  if (pattern.empty()) return false;
  std::string postfix_pattern = GetPostfixExpr(pattern);
  std::stringstream assign_expr_ss;
  assign_expr_ss << " " << assignment_stmt->get_expr_string() << " ";
  std::string assign_expr = assign_expr_ss.str();
  if (is_partial_match) {
    return assign_expr.find(postfix_pattern) != std::string::npos;
  } else {
    return assign_expr == postfix_pattern;
  }
}

bool PatternManager::TestIfWhilePattern(Statement *stmt, std::string variable) {
  if (variable.size() == 0) return false;
  std::stringstream expr_ss, variable_ss;
  variable_ss << " " << variable << " ";
  expr_ss << " " << stmt->get_expr_string() << " ";
  std::string stmt_expr = expr_ss.str();
  return stmt_expr.find(variable_ss.str()) != std::string::npos;
}

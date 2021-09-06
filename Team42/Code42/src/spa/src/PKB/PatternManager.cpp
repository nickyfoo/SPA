#include "PatternManager.h"

#include <iostream>
#include <unordered_map>
#include <stack>
#include <sstream>

auto checkNextChar(bool isIdent);

// Returns a curried function that checks for validity of the next character
// depending on the type of variable.
auto checkNextChar(bool isIdent) {
    return [=](char c) {
        return isIdent ? std::isalnum(c) : std::isdigit(c);
    };
}

// Returns an infix expression in postfix notation
std::string PatternManager::getPostfixExpr(std::string infixExpr) {
    try {
        std::stack<char> ops;
        std::stringstream output;
        std::string::iterator it;

        for (it = infixExpr.begin(); it != infixExpr.end(); ++it) {
            if (std::isalnum(*it)) {
                // Flag for whether variable is an identifier or a constant
                bool isIdent = std::isalpha(*it);
                output << *it;

                // Advance iterator if next character is valid
                while (std::next(it, 1) != infixExpr.end() &&
                       checkNextChar(isIdent)(*std::next(it, 1))) {
                    std::advance(it, 1);
                    output << *it;
                }
                output << ' ';
                continue;
            }

            switch (*it) {
                case '(':
                    ops.push(*it);
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
                case '/':
                {
                    if (ops.empty() || ops.top() == '(') {
                        ops.push(*it);
                        break;
                    }

                    std::unordered_map<char, int>::const_iterator lastOps = opsMap.find(ops.top());
                    std::unordered_map<char, int>::const_iterator newOps = opsMap.find(*it);
                    if (newOps->second < lastOps->second) {
                        // Case 1: New operand has a higher precedence than the top of the stack
                        ops.push(*it);
                    } else if (newOps->second > lastOps->second) {
                        // Case 2: New operand has a lower precedence than the top of the stack
                        while (newOps->second >= lastOps->second && !ops.empty()) {
                            // Pop stack while precedence of the new operand is lte the top of the stack
                            output << ops.top() << ' ';
                            ops.pop();
                            if (ops.empty()) break;
                            lastOps = opsMap.find(ops.top());
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
                case ' ':
                    break;
                default:
                    throw std::invalid_argument("Expression is not valid");
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
        return output.str();
    } catch (std::invalid_argument e) {
        std::cerr << e.what() << std::endl;

    }
}

bool PatternManager::checkAssignmentRhs(Statement* assignmentStmt, std::string pattern, bool isPartialMatch) {
    std::cout << "\n";
    std::string postfixPattern = getPostfixExpr(pattern);
    postfixPattern.pop_back();
    std::string assignmentExpr = assignmentStmt->getExprString();
    // std::cout << "assignmentExpr: " << assignmentExpr << "\n";
    // std::cout << "postfixPattern: " << postfixPattern << " --> " << (assignmentExpr == postfixPattern) << "\n";
    if (isPartialMatch) {
        return assignmentExpr.find(postfixPattern) != std::string::npos;
    } else {

        return assignmentExpr == postfixPattern;
    }
}

#include "PatternManager.h"

#include <iostream>
#include <unordered_map>
#include <stack>
#include <sstream>

std::string PatternManager::getPostfixExpr(std::string infixExpr) {
    std::stack<char> ops;
    std::stringstream soutput;

    std::string::iterator it;
    for (it = infixExpr.begin(); it != infixExpr.end(); ++it) {
        if (std::isalpha(*it)) {
            soutput << *it;
            while (std::isalnum(*std::next(it, 1))) {
                std::advance(it, 1);
                soutput << *it;
            }

            soutput << ' ';
            continue;
        }
        switch (*it) {
            case '(':
                ops.push(*it);
                break;
            case ')':
                while (ops.top() != '(' && !ops.empty()) {
                    soutput << ops.top() << ' ';
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
                } else {
                    std::unordered_map<char, int>::const_iterator lastOps = opsMap.find(ops.top());
                    std::unordered_map<char, int>::const_iterator newOps = opsMap.find(*it);
                    if (newOps->second < lastOps->second) {
                        ops.push(*it);
                    } else if (newOps->second > lastOps->second) {
                        soutput << ops.top() << ' ';
                        ops.pop();
                        if (ops.empty()) {
                            ops.push(*it);
                            break;
                        }
                        do {
                            lastOps = opsMap.find(ops.top());
                            if (newOps->second > lastOps->second) {
                                soutput << ops.top() << ' ';
                                ops.pop();
                            } else if (newOps->second < lastOps->second) {
                                ops.push(*it);
                            } else {
                                soutput << ops.top() << ' ';
                                ops.pop();
                                ops.push(*it);
                            }
                        } while (lastOps->second < newOps->second && !ops.empty());
                    } else {
                        // if in the same level, pop the ops and add it to the output
                        // push new operand in
                        soutput << ops.top() << ' ';
                        ops.pop();
                        ops.push(*it);
                    }
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
        soutput << ops.top() << ' ';
        ops.pop();
    }
    return soutput.str();
}

bool PatternManager::checkAssignmentRhs(Statement* assignmentStmt, std::string pattern, bool isPartialMatch) {
    try {
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

    } catch (std::invalid_argument e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

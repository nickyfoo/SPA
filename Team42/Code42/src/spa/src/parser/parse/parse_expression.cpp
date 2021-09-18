#include <iostream>
#include <stack>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"
#include "lexer.h"
#include "parse.h"
#include "string_utils.h"

std::variant<ExpressionNode *, ConstantNode *, IdentifierNode *> ParseExpression(
    BufferedLexer *lexer, ParseState *state) {
  // use shunting yard algorithm to parse expressions
  std::stack<std::variant<ExpressionNode *, ConstantNode *, IdentifierNode *>> result_stack{};
  std::stack<const Token *> operator_stack{};

  auto *next_token = lexer->PeekNextToken();
  while (IsExpressionToken(next_token)) {
    switch (next_token->kind_) {
      case TokenType::Number:
        result_stack.push(MakeConstantNodeFromToken(next_token));
        break;

      case TokenType::Name:
        result_stack.push(MakeIdentifierNodeFromToken(next_token));
        break;

      case TokenType::LParen:
        operator_stack.push(next_token);
        break;

      case TokenType::RParen:
        // clear all operators till matching LParen found
        const Token *op_token;
        while (!operator_stack.empty() &&
               (op_token = operator_stack.top())->kind_ != TokenType::LParen) {
          operator_stack.pop();

          if (result_stack.size() < 2) {
            throw ParseException(
                StringFormat("missing operands for '%s' expression", op_token->value_),
                op_token->line_no_, op_token->col_no_);
          }

          ExpressionNodeChild *left, *right;
          auto next_res = result_stack.top();
          result_stack.pop();
          if (std::holds_alternative<ConstantNode *>(next_res)) {
            left = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
          } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
            left = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
          } else {
            left = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
          }
          next_res = result_stack.top();
          result_stack.pop();
          if (std::holds_alternative<ConstantNode *>(next_res)) {
            right = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
          } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
            right = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
          } else {
            right = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
          }

          auto res = new ExpressionNode(ExprOpFromToken(op_token), left, right,
                                        {left->get_line_no(), left->get_col_no()});
          result_stack.push(res);
        }

        // we expect a LParen at top of stack
        if (operator_stack.empty()) {
          throw ParseException("matching '(' not found", next_token->line_no_, next_token->col_no_);
        }
        break;

      default:
        // clear all operators with higher precedence
        while (!operator_stack.empty() &&
               (op_token = operator_stack.top())->kind_ != TokenType::LParen &&
               Precedes(op_token, next_token)) {
          operator_stack.pop();
          if (result_stack.size() < 2) {
            throw ParseException(
                StringFormat("missing operands for '%s' expression", op_token->value_),
                op_token->line_no_, op_token->col_no_);
          }

          ExpressionNodeChild *left, *right;
          auto next_res = result_stack.top();
          result_stack.pop();
          if (std::holds_alternative<ConstantNode *>(next_res)) {
            left = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
          } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
            left = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
          } else {
            left = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
          }
          next_res = result_stack.top();
          result_stack.pop();
          if (std::holds_alternative<ConstantNode *>(next_res)) {
            right = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
          } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
            right = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
          } else {
            right = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
          }

          auto res = new ExpressionNode(ExprOpFromToken(op_token), left, right,
                                        {left->get_line_no(), left->get_col_no()});
          result_stack.push(res);
        }
        operator_stack.push(next_token);
    }
    lexer->GetNextToken();
    next_token = lexer->PeekNextToken();
  }

  while (!operator_stack.empty()) {
    auto *op_token = operator_stack.top();
    operator_stack.pop();

    // there's a chance that there might be mismatched LParen left in operator stack
    if (op_token->kind_ == TokenType::LParen) {
      throw ParseException("matching ')' not found", op_token->line_no_, op_token->col_no_);
    }

    ExpressionNodeChild *left, *right;
    auto next_res = result_stack.top();
    result_stack.pop();
    if (std::holds_alternative<ConstantNode *>(next_res)) {
      left = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
    } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
      left = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
    } else {
      left = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
    }
    next_res = result_stack.top();
    result_stack.pop();
    if (std::holds_alternative<ConstantNode *>(next_res)) {
      right = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
    } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
      right = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
    } else {
      right = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
    }

    auto res = new ExpressionNode(ExprOpFromToken(op_token), left, right,
                                  {left->get_line_no(), left->get_col_no()});
    result_stack.push(res);
  }

  // now we should only have one tree in the results stack
  auto final_res = result_stack.top();
  result_stack.pop();

  if (result_stack.size() > 0) {
    // annoying but we need to get the first element of stack to get the start location
    std::variant<ExpressionNode *, ConstantNode *, IdentifierNode *> next_res;
    while (!result_stack.empty()) {
      next_res = result_stack.top();
      result_stack.pop();
    }
    ExpressionNodeChild *first_res;
    if (std::holds_alternative<ConstantNode *>(next_res)) {
      first_res = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
    } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
      first_res = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
    } else {
      first_res = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
    }
    throw ParseException("invalid expression", first_res->get_line_no(), first_res->get_col_no());
  }

  return final_res;
}

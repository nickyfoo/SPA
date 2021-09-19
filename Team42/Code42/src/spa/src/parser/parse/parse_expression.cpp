#include <cassert>
#include <iostream>
#include <stack>
#include <stdexcept>

#include "ast.h"
#include "ast_utils.hpp"
#include "lexer.h"
#include "parse.h"

std::variant<CondExpressionNode *, RelExpressionNode *, ExpressionNode *, ConstantNode *,
             IdentifierNode *>
ParseExpression(BufferedLexer *lexer, ParseState *state, std::string end) {
  // use shunting yard algorithm to parse expressions
  std::stack<std::variant<CondExpressionNode *, RelExpressionNode *, ExpressionNode *,
                          ConstantNode *, IdentifierNode *>>
      result_stack{};
  std::stack<const Token *> operator_stack{};

  auto *next_token = lexer->PeekNextToken();
  while (IsExpressionToken(next_token)) {
    if (next_token->value_ == end && operator_stack.empty() && result_stack.size() == 1) {
      // terminate early
      return result_stack.top();
    }

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

          if (IsExprOp(op_token->kind_)) {
            if (result_stack.size() < 2) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            ExpressionNodeChild *left, *right;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<ConstantNode *>(next_res)) {
              right = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              right = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              right = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }
            next_res = result_stack.top();
            result_stack.pop();
            if (std::holds_alternative<ConstantNode *>(next_res)) {
              left = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              left = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              left = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new ExpressionNode(ExprOpFromToken(op_token), left, right,
                                          {left->get_line_no(), left->get_col_no()});
            result_stack.push(res);
          } else if (IsRelExprOp(op_token->kind_)) {
            if (result_stack.size() < 2) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            RelExpressionNodeChild *left, *right;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<ConstantNode *>(next_res)) {
              right = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              right = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              right = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }
            next_res = result_stack.top();
            result_stack.pop();
            if (std::holds_alternative<ConstantNode *>(next_res)) {
              left = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              left = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              left = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new RelExpressionNode(RelExprOpFromToken(op_token), left, right,
                                             {left->get_line_no(), left->get_col_no()});
            result_stack.push(res);
          } else if (op_token->kind_ == TokenType::Not) {
            if (result_stack.size() < 1) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            CondExpressionNodeChild *left;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<CondExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
            } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new CondExpressionNode(CondExprOpFromToken(op_token), left, nullptr,
                                              {op_token->line_no_, op_token->col_no_});
            result_stack.push(res);
          } else if (IsCondExprOp(op_token->kind_)) {
            // And or Or
            if (result_stack.size() < 2) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            CondExpressionNodeChild *left, *right;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<CondExpressionNode *>(next_res)) {
              right =
                  static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
            } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
              right =
                  static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }
            next_res = result_stack.top();
            result_stack.pop();
            if (std::holds_alternative<CondExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
            } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new CondExpressionNode(CondExprOpFromToken(op_token), left, right,
                                              {left->get_line_no(), left->get_col_no()});
            result_stack.push(res);
          } else {
            // should never reach here
            assert(false);
          }
        }

        // we expect a LParen at top of stack
        if (operator_stack.empty()) {
          throw ParseException("matching '(' not found", next_token->line_no_, next_token->col_no_);
        }
        operator_stack.pop();
        break;

      default:
        // clear all operators with higher precedence
        while (!operator_stack.empty() &&
               (op_token = operator_stack.top())->kind_ != TokenType::LParen &&
               Precedes(op_token->kind_, next_token->kind_)) {
          operator_stack.pop();

          if (IsExprOp(op_token->kind_)) {
            if (result_stack.size() < 2) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            ExpressionNodeChild *left, *right;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<ConstantNode *>(next_res)) {
              right = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              right = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              right = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }
            next_res = result_stack.top();
            result_stack.pop();
            if (std::holds_alternative<ConstantNode *>(next_res)) {
              left = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              left = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              left = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new ExpressionNode(ExprOpFromToken(op_token), left, right,
                                          {left->get_line_no(), left->get_col_no()});
            result_stack.push(res);
          } else if (IsRelExprOp(op_token->kind_)) {
            if (result_stack.size() < 2) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            RelExpressionNodeChild *left, *right;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<ConstantNode *>(next_res)) {
              right = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              right = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              right = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }
            next_res = result_stack.top();
            result_stack.pop();
            if (std::holds_alternative<ConstantNode *>(next_res)) {
              left = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
            } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
              left = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
            } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
              left = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new RelExpressionNode(RelExprOpFromToken(op_token), left, right,
                                             {left->get_line_no(), left->get_col_no()});
            result_stack.push(res);
          } else if (op_token->kind_ == TokenType::Not) {
            if (result_stack.size() < 1) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            CondExpressionNodeChild *left;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<CondExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
            } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new CondExpressionNode(CondExprOpFromToken(op_token), left, nullptr,
                                              {op_token->line_no_, op_token->col_no_});
            result_stack.push(res);
          } else if (IsCondExprOp(op_token->kind_)) {
            // And or Or
            if (result_stack.size() < 2) {
              throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                                   op_token->line_no_, op_token->col_no_);
            }

            CondExpressionNodeChild *left, *right;
            auto next_res = result_stack.top();
            result_stack.pop();

            if (std::holds_alternative<CondExpressionNode *>(next_res)) {
              right =
                  static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
            } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
              right =
                  static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }
            next_res = result_stack.top();
            result_stack.pop();
            if (std::holds_alternative<CondExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
            } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
              left =
                  static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
            } else {
              throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                                   op_token->line_no_, op_token->col_no_);
            }

            auto res = new CondExpressionNode(CondExprOpFromToken(op_token), left, right,
                                              {left->get_line_no(), left->get_col_no()});
            result_stack.push(res);
          } else {
            // should never reach here
            assert(false);
          }
        }

        operator_stack.push(next_token);
    }
    lexer->GetNextToken();
    next_token = lexer->PeekNextToken();
  }

  while (!operator_stack.empty()) {
    auto op_token = operator_stack.top();
    operator_stack.pop();

    // there's a chance that there might be mismatched LParen left in operator stack
    if (op_token->kind_ == TokenType::LParen) {
      throw ParseException("matching ')' not found", op_token->line_no_, op_token->col_no_);
    }

    if (IsExprOp(op_token->kind_)) {
      if (result_stack.size() < 2) {
        throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                             op_token->line_no_, op_token->col_no_);
      }

      ExpressionNodeChild *left, *right;
      auto next_res = result_stack.top();
      result_stack.pop();

      if (std::holds_alternative<ConstantNode *>(next_res)) {
        right = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
      } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
        right = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
      } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
        right = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }
      next_res = result_stack.top();
      result_stack.pop();
      if (std::holds_alternative<ConstantNode *>(next_res)) {
        left = static_cast<ExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
      } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
        left = static_cast<ExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
      } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
        left = static_cast<ExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }

      auto res = new ExpressionNode(ExprOpFromToken(op_token), left, right,
                                    {left->get_line_no(), left->get_col_no()});
      result_stack.push(res);
    } else if (IsRelExprOp(op_token->kind_)) {
      if (result_stack.size() < 2) {
        throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                             op_token->line_no_, op_token->col_no_);
      }

      RelExpressionNodeChild *left, *right;
      auto next_res = result_stack.top();
      result_stack.pop();

      if (std::holds_alternative<ConstantNode *>(next_res)) {
        right = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
      } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
        right = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
      } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
        right = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }
      next_res = result_stack.top();
      result_stack.pop();
      if (std::holds_alternative<ConstantNode *>(next_res)) {
        left = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(next_res));
      } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
        left = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(next_res));
      } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
        left = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }

      auto res = new RelExpressionNode(RelExprOpFromToken(op_token), left, right,
                                       {left->get_line_no(), left->get_col_no()});
      result_stack.push(res);
    } else if (op_token->kind_ == TokenType::Not) {
      if (result_stack.size() < 1) {
        throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                             op_token->line_no_, op_token->col_no_);
      }

      CondExpressionNodeChild *left;
      auto next_res = result_stack.top();
      result_stack.pop();

      if (std::holds_alternative<CondExpressionNode *>(next_res)) {
        left = static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
      } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
        left = static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }

      auto res = new CondExpressionNode(CondExprOpFromToken(op_token), left, nullptr,
                                        {op_token->line_no_, op_token->col_no_});
      result_stack.push(res);
    } else if (IsCondExprOp(op_token->kind_)) {
      // And or Or
      if (result_stack.size() < 2) {
        throw ParseException("missing operands for '" + op_token->value_ + "' expresion",
                             op_token->line_no_, op_token->col_no_);
      }

      CondExpressionNodeChild *left, *right;
      auto next_res = result_stack.top();
      result_stack.pop();

      if (std::holds_alternative<CondExpressionNode *>(next_res)) {
        right = static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
      } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
        right = static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }
      next_res = result_stack.top();
      result_stack.pop();
      if (std::holds_alternative<CondExpressionNode *>(next_res)) {
        left = static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(next_res));
      } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
        left = static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(next_res));
      } else {
        throw ParseException("invalid operands for '" + op_token->value_ + "' operator",
                             op_token->line_no_, op_token->col_no_);
      }

      auto res = new CondExpressionNode(CondExprOpFromToken(op_token), left, right,
                                        {left->get_line_no(), left->get_col_no()});
      result_stack.push(res);
    } else {
      // should never reach here
      assert(false);
    }
  }

  // now we should only have one tree in the results stack
  auto final_res = result_stack.top();
  result_stack.pop();

  if (result_stack.size() > 0) {
    // annoying but we need to get the first element of stack to get the start location
    std::variant<CondExpressionNode *, RelExpressionNode *, ExpressionNode *, ConstantNode *,
                 IdentifierNode *>
        next_res;
    while (!result_stack.empty()) {
      next_res = result_stack.top();
      result_stack.pop();
    }
    if (std::holds_alternative<ConstantNode *>(next_res)) {
      auto first_res = std::get<ConstantNode *>(next_res);
      throw ParseException("invalid expression", first_res->get_line_no(), first_res->get_col_no());
    } else if (std::holds_alternative<IdentifierNode *>(next_res)) {
      auto first_res = std::get<IdentifierNode *>(next_res);
      throw ParseException("invalid expression", first_res->get_line_no(), first_res->get_col_no());
    } else if (std::holds_alternative<ExpressionNode *>(next_res)) {
      auto first_res = std::get<ExpressionNode *>(next_res);
      throw ParseException("invalid expression", first_res->get_line_no(), first_res->get_col_no());
    } else if (std::holds_alternative<RelExpressionNode *>(next_res)) {
      auto first_res = std::get<RelExpressionNode *>(next_res);
      throw ParseException("invalid expression", first_res->get_line_no(), first_res->get_col_no());
    } else {
      auto first_res = std::get<CondExpressionNode *>(next_res);
      throw ParseException("invalid expression", first_res->get_line_no(), first_res->get_col_no());
    }
  }

  return final_res;
}

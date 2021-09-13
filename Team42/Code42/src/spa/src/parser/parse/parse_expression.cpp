#include <iostream>
#include <stack>
#include <stdexcept>

#include "ast.h"
#include "lexer.h"
#include "parse.h"
#include "string_utils.h"

bool IsExpressionToken(const Token *t) {
  switch (t->kind_) {
    case TokenType::Constant:
    case TokenType::Identifier:
    case TokenType::LParen:
    case TokenType::RParen:
    case TokenType::Plus:
    case TokenType::Minus:
    case TokenType::Multiply:
    case TokenType::Divide:
    case TokenType::Modulo:
      return true;
    default:
      return false;
  }
}

ExprOp ExprOpFromToken(const Token *t) {
  switch (t->kind_) {
    case TokenType::Plus:
      return ExprOp::Plus;
    case TokenType::Minus:
      return ExprOp::Minus;
    case TokenType::Multiply:
      return ExprOp::Times;
    case TokenType::Divide:
      return ExprOp::Divide;
    case TokenType::Modulo:
      return ExprOp::Modulo;
    default:
      throw std::invalid_argument(
          "ExprOpFromToken: expected token kind to be Plus, Minus, Multiply, "
          "Times, Divide or Modulo");
  }
}

std::string ExprOpToString(ExprOp op) {
  switch (op) {
    case ExprOp::Plus:
      return "+";
    case ExprOp::Minus:
      return "-";
    case ExprOp::Times:
      return "*";
    case ExprOp::Divide:
      return "/";
    case ExprOp::Modulo:
      return "%";
    default:
      throw std::invalid_argument("ExprOpToString: invalid ExprOp value");
  }
}

bool Precedes(const Token *op1, const Token *op2) {
  // return true if t1 has greater or equal precedence than t2
  switch (op1->kind_) {
    case TokenType::Multiply:
    case TokenType::Divide:
    case TokenType::Modulo:
      return true;

    case TokenType::Plus:
    case TokenType::Minus:
      switch (op2->kind_) {
        case TokenType::Plus:
        case TokenType::Minus:
          return true;

        case TokenType::Multiply:
        case TokenType::Divide:
        case TokenType::Modulo:
          return false;

        default:
          throw std::invalid_argument("Precedes: invalid operator token for op2");
      }

    default:
      throw std::invalid_argument("Precedes: invalid operator token for op1");
  }
}

ConstantNode *MakeConstantNodeFromToken(const Token *t) {
  return new ConstantNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
};

IdentifierNode *MakeIdentifierNodeFromToken(const Token *t) {
  return new IdentifierNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
};

void PopExpressionNodeFromStacks(const Token *op, std::stack<Node *> *result_stack,
                                 std::stack<std::string> *expr_str_stack) {
  ExprOp expr_op = ExprOpFromToken(op);
  std::string expr_op_str = ExprOpToString(expr_op);

  if (result_stack->size() < 2) {
    std::string err_msg = StringFormat("expected operand after %s", expr_op_str.c_str());
  }

  // create op string of new node
  std::string right_str = expr_str_stack->top();
  expr_str_stack->pop();
  std::string left_str = expr_str_stack->top();
  expr_str_stack->pop();

  // create new node
  Node *right = result_stack->top();
  result_stack->pop();
  Node *left = result_stack->top();
  result_stack->pop();

  std::string expr_str = StringFormat("%s %s %s", left_str.c_str(), right_str.c_str(), expr_op_str.c_str());
  LocInfo loc = LocInfo{.line_no = left->get_line_no(), .col_no = left->get_col_no()};

  result_stack->push(new ExpressionNode(expr_op, left, right, expr_str, loc));
  expr_str_stack->push(expr_str);
}

void ProcessRightParen(const Token *right_paren, std::stack<Node *> *result_stack,
                       std::stack<std::string> *expr_str_stack,
                       std::stack<const Token *> *operator_stack) {
  // clear remaning operators before the LParen
  const Token *op;
  while (!operator_stack->empty() && (op = operator_stack->top())->kind_ != TokenType::LParen) {
    PopExpressionNodeFromStacks(op, result_stack, expr_str_stack);
    operator_stack->pop();
  }

  // we expect a LParen at the top of the operator queue
  if (operator_stack->empty()) {
    throw ParseException("matching parenthesis not found", right_paren->line_no_,
                         right_paren->col_no_);
  }

  operator_stack->pop();
}

void ProcessOperator(const Token *op, std::stack<Node *> *result_stack,
                     std::stack<std::string> *expr_str_stack,
                     std::stack<const Token *> *operator_stack) {
  // clear operators with higher precedence
  const Token *top;
  while (!operator_stack->empty() && (top = operator_stack->top())->kind_ != TokenType::LParen &&
         Precedes(top, op)) {
    PopExpressionNodeFromStacks(top, result_stack, expr_str_stack);
    operator_stack->pop();
  }
  operator_stack->push(op);
}

Node *ParseExpression(BufferedLexer *lexer, ParseState *state) {
  // use shunting yard algorithm to parse expressions
  std::stack<Node *> result_stack{};
  std::stack<std::string> expr_str_stack{};
  std::stack<const Token *> operator_stack{};

  const Token *next_token = lexer->PeekNextToken();
  while (IsExpressionToken(next_token)) {
    switch (next_token->kind_) {
      case TokenType::Constant:
        result_stack.push(MakeConstantNodeFromToken(next_token));
        expr_str_stack.push(next_token->value_);
        break;

      case TokenType::Identifier:
        result_stack.push(MakeIdentifierNodeFromToken(next_token));
        expr_str_stack.push(next_token->value_);
        break;

      case TokenType::LParen:
        operator_stack.push(next_token);
        break;

      case TokenType::RParen:
        ProcessRightParen(next_token, &result_stack, &expr_str_stack, &operator_stack);
        break;

      default:
        // must be an operator
        ProcessOperator(next_token, &result_stack, &expr_str_stack, &operator_stack);
    }

    // flush token
    lexer->GetNextToken();
    next_token = lexer->PeekNextToken();
  }

  // now we clear all outstanding operators and sub-results
  // flush the token and peek next
  while (!operator_stack.empty()) {
    const Token *op = operator_stack.top();

    // there's a chance that there might be mismatched LParen left in operator stack
    if (op->kind_ == TokenType::LParen) {
      throw ParseException("matching parenthesis not found", op->line_no_, op->col_no_);
    }

    PopExpressionNodeFromStacks(op, &result_stack, &expr_str_stack);
    operator_stack.pop();
  }

  // now we should only have one tree in the results stack
  Node *final_res = result_stack.top();
  result_stack.pop();
  if (result_stack.size() > 0) {
    throw ParseException("unexpected expression found", final_res->get_line_no(), final_res->get_col_no());
  }

  return final_res;
}


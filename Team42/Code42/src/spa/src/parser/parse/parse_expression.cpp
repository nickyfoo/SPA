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
          "ExprOpFromToken: expected token kind to be Plus, Minus, Multiply, Times, Divide or "
          "Modulo");
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

ConstantNode *MakeConstantNodeFromToken(const Token *t) {
  return new ConstantNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
};

IdentifierNode *MakeIdentifierNodeFromToken(const Token *t) {
  return new IdentifierNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
};

void ProcessRightParen(const Token *right_paren, std::stack<Node *> *result_stack,
                       std::stack<std::string> *expr_str_stack,
                       std::stack<const Token *> *operator_stack) {
  const Token *op;
  while (!operator_stack->empty() && (op = operator_stack->top())->kind_ != TokenType::LParen) {
    operator_stack->pop();

    if (operator_stack->empty()) {
      // we are at the last element of the operator_stack, but still no LParen token
      throw ParseException("no matching parenthesis", right_paren->line_no_, right_paren->col_no_);
    }

    // clear remaning operators before the LParen
    ExprOp expr_op = ExprOpFromToken(op);

    // gen op string of new node
    std::string expr_op_str = ExprOpToString(expr_op);
    std::string left_str = expr_str_stack->top();
    expr_str_stack->pop();
    std::string right_str = expr_str_stack->top();
    expr_str_stack->pop();

    // gen new node
    Node *right = result_stack->top();
    result_stack->pop();
    Node *left = result_stack->top();
    result_stack->pop();

    std::string expr_str = StringFormat("%s %s %s", left_str, right_str, expr_op_str);
    ExpressionNode *sub_result = new ExpressionNode(expr_op, left, right, expr_str,

    result_stack->push()
  }
}

void ProcessOperator(std::stack<Node *> *result_stack, std::stack<std::string> *expr_str_stack,
                     std::stack<const Token *> *operator_stack) {}

ExpressionNode *ParseExpression(BufferedLexer *lexer, ParseState *state) {
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

      case TokenType::Identifier:
        result_stack.push(MakeIdentifierNodeFromToken(next_token));
        expr_str_stack.push(next_token->value_);

      case TokenType::LParen:
        operator_stack.push(next_token);

      case TokenType::RParen:
        ProcessRightParen(next_token, &result_stack, &expr_str_stack, &operator_stack);

      default:
        // must be an opoerator
        ProcessOperator(&result_stack, &expr_str_stack, &operator_stack);
    }

    // flush token
    lexer->GetNextToken();
    next_token = lexer->PeekNextToken();
  }

  lexer::Token *t = lexer->peekNextToken();
  while (isExpressionToken(t)) {
    if (t->kind == lexer::Kind::Constant) {
      outputStack.push(new ast::ConstantNode{t->value, t->lineNo, t->colNo});
      exprStringStack.push(t->value);

    } else if (t->kind == lexer::Kind::Identifier) {
      outputStack.push(new ast::IdentifierNode{t->value, t->lineNo, t->colNo});
      exprStringStack.push(t->value);

    } else if (t->kind == lexer::Kind::LParen) {
      operatorStack.push(t);

    } else if (t->kind == lexer::Kind::RParen) {
      lexer::Token *op;
      while (!operatorStack.empty() && (op = operatorStack.top())->kind != lexer::Kind::LParen) {
        operatorStack.pop();

        if (operatorStack.empty()) {
          throw parseError("matching parenthesis", t->lineNo, t->colNo);
        }

        if (outputStack.size() < 2) {
          throw parseError("missing operand", op->lineNo, op->colNo);
        }

        ast::Node *right = outputStack.top();
        outputStack.pop();
        ast::Node *left = outputStack.top();
        outputStack.pop();
        ast::ExprOp exprOp = exprOpFromToken(op);

        std::string exprString = exprStringStack.top();
        exprStringStack.pop();
        exprString += " " + exprStringStack.top();
        exprStringStack.pop();
        exprString += " " + exprOpStringFromToken(op);

        outputStack.push(
            new ast::ExpressionNode{exprOp, left, right, exprString, op->lineNo, op->colNo});
        exprStringStack.push(exprString);
      }

      // flush matching (
      operatorStack.pop();
    } else {
      // t is an operator
      lexer::Token *op;
      while (!operatorStack.empty() && (op = operatorStack.top())->kind != lexer::Kind::LParen &&
             precedes(op, t)) {
        operatorStack.pop();

        if (outputStack.size() < 2) {
          throw parseError("missing operand", op->lineNo, op->colNo);
        }

        ast::Node *right = outputStack.top();
        outputStack.pop();
        ast::Node *left = outputStack.top();
        outputStack.pop();
        ast::ExprOp exprOp = exprOpFromToken(op);

        std::string exprString = exprStringStack.top();
        exprStringStack.pop();
        exprString = exprStringStack.top() + " " + exprString;
        exprStringStack.pop();
        exprString += " " + exprOpStringFromToken(op);

        outputStack.push(
            new ast::ExpressionNode{exprOp, left, right, exprString, op->lineNo, op->colNo});
        exprStringStack.push(exprString);
      }

      operatorStack.push(t);
    }

    // flush the token and peek next
    lexer->getNextToken();
    t = lexer->peekNextToken();
  }

  while (!operatorStack.empty()) {
    lexer::Token *op = operatorStack.top();
    operatorStack.pop();

    if (outputStack.size() < 2) {
      throw parseError("missing operand", op->lineNo, op->colNo);
    }

    ast::Node *right = outputStack.top();
    outputStack.pop();
    ast::Node *left = outputStack.top();
    outputStack.pop();
    ast::ExprOp exprOp = exprOpFromToken(op);

    std::string exprString = exprStringStack.top();
    exprStringStack.pop();
    exprString = exprStringStack.top() + " " + exprString;
    exprStringStack.pop();
    exprString += " " + exprOpStringFromToken(op);

    outputStack.push(
        new ast::ExpressionNode{exprOp, left, right, exprString, op->lineNo, op->colNo});
    exprStringStack.push(exprString);
  }

  ast::Node *n = outputStack.top();
  outputStack.pop();

  if (outputStack.size() > 0) {
    throw parseError("dangling operand", outputStack.top()->lineNo, outputStack.top()->colNo);
  }

  if (n->kind == ast::Kind::Expression) {
    ast::ExpressionNode *res = static_cast<ast::ExpressionNode *>(n);
    return res;
  }

  if (n->kind == ast::Kind::Constant) {
    ast::ConstantNode *c = static_cast<ast::ConstantNode *>(n);
    return new ast::ExpressionNode{ast::ExprOp::Noop, n, nullptr, c->value, n->lineNo, n->colNo};
  }

  if (n->kind == ast::Kind::Identifier) {
    ast::IdentifierNode *i = static_cast<ast::IdentifierNode *>(n);
    return new ast::ExpressionNode{ast::ExprOp::Noop, n, nullptr, i->name, n->lineNo, n->colNo};
  }

  throw parseError("expression, constant or identifier", n->lineNo, n->colNo);
}

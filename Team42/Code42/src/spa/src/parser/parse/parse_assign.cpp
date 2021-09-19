#include "parse.h"

AssignNode *ParseAssign(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name) {
    throw ParseException("expected name but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  auto var = new IdentifierNode(t->value_, {t->line_no_, t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Equal) {
    throw ParseException("expected '=' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  auto parse_result = ParseExpression(lexer, state, "");
  AssignNodeExpr *expr;
  if (std::holds_alternative<ConstantNode *>(parse_result)) {
    expr = static_cast<AssignNodeExpr *>(std::get<ConstantNode *>(parse_result));
  } else if (std::holds_alternative<IdentifierNode *>(parse_result)) {
    expr = static_cast<AssignNodeExpr *>(std::get<IdentifierNode *>(parse_result));
  } else if (std::holds_alternative<ExpressionNode *>(parse_result)) {
    expr = static_cast<AssignNodeExpr *>(std::get<ExpressionNode *>(parse_result));
  } else if (std::holds_alternative<RelExpressionNode *>(parse_result)) {
    auto node = std::get<RelExpressionNode *>(parse_result);
    throw ParseException("expected Expression but got RelExpression", node->get_line_no(),
                         node->get_col_no());
  } else {
    auto node = std::get<CondExpressionNode *>(parse_result);
    throw ParseException("expected Expression but got CondExpression", node->get_line_no(),
                         node->get_col_no());
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException("expected ';' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  return new AssignNode(var, expr, stmt_no, {start_line, start_col});
}

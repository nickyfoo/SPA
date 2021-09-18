#include "parse.h"
#include "string_utils.h"

AssignNode *ParseAssign(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name) {
    throw ParseException(StringFormat("expected name but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  auto var = new IdentifierNode(t->value_, {t->line_no_, t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Equal) {
    throw ParseException(StringFormat("expected '=' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  auto parse_result = ParseExpression(lexer, state);
  AssignNodeExpr *expr;
  if (std::holds_alternative<ConstantNode *>(parse_result)) {
    expr = static_cast<AssignNodeExpr *>(std::get<ConstantNode *>(parse_result));
  } else if (std::holds_alternative<IdentifierNode *>(parse_result)) {
    expr = static_cast<AssignNodeExpr *>(std::get<IdentifierNode *>(parse_result));
  } else {
    expr = static_cast<AssignNodeExpr *>(std::get<ExpressionNode *>(parse_result));
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException(StringFormat("expected ';' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  return new AssignNode(var, expr, stmt_no, {start_line, start_col});
}

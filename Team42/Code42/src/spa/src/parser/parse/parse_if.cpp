#include <variant>

#include "parse.h"
#include "string_utils.h"

IfNode *ParseIf(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name && t->value_ != "if") {
    throw ParseException(StringFormat("expected 'if' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  auto parse_result = ParseCondExpression(lexer, state);
  IfWhileNodeCond *cond;
  if (std::holds_alternative<RelExpressionNode *>(parse_result)) {
    cond = static_cast<IfWhileNodeCond *>(std::get<RelExpressionNode *>(parse_result));
  } else {
    cond = static_cast<IfWhileNodeCond *>(std::get<CondExpressionNode *>(parse_result));
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Name && t->value_ != "then") {
    throw ParseException(StringFormat("expected 'then' but got '%s'", t->value_.c_str()),
                         t->line_no_, t->col_no_);
  }

  std::vector<StatementNode *> then_stmt_lst = ParseStmtLst(lexer, state);

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Name && t->value_ != "else") {
    throw ParseException(StringFormat("expected 'else' but got '%s'", t->value_.c_str()),
                         t->line_no_, t->col_no_);
  }

  std::vector<StatementNode *> else_stmt_lst = ParseStmtLst(lexer, state);

  return new IfNode(cond, then_stmt_lst, else_stmt_lst, stmt_no, {start_line, start_col});
}

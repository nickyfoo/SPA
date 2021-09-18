#include "parse.h"
#include "string_utils.h"

WhileNode *ParseWhile(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name || t->value_ != "while") {
    throw ParseException(StringFormat("expected 'while' but got '%s'", t->value_.c_str()),
                         t->line_no_, t->col_no_);
  }

  auto parse_result = ParseCondExpression(lexer, state);
  IfWhileNodeCond *cond;
  if (std::holds_alternative<RelExpressionNode *>(parse_result)) {
    cond = static_cast<IfWhileNodeCond *>(std::get<RelExpressionNode *>(parse_result));
  } else {
    cond = static_cast<IfWhileNodeCond *>(std::get<CondExpressionNode *>(parse_result));
  }

  std::vector<StatementNode *> stmt_lst = ParseStmtLst(lexer, state);

  return new WhileNode(cond, stmt_lst, stmt_no, {start_line, start_col});
}

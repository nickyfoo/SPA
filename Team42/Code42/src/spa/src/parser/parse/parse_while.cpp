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

  auto parse_result = ParseExpression(lexer, state, "");
  IfWhileNodeCond *cond;
  if (std::holds_alternative<RelExpressionNode *>(parse_result)) {
    cond = static_cast<IfWhileNodeCond *>(std::get<RelExpressionNode *>(parse_result));
  } else if (std::holds_alternative<CondExpressionNode *>(parse_result)) {
    cond = static_cast<IfWhileNodeCond *>(std::get<CondExpressionNode *>(parse_result));
  } else if (std::holds_alternative<ExpressionNode *>(parse_result)) {
    auto node = std::get<ExpressionNode *>(parse_result);
    throw ParseException("expected CondExpression but got Expression", node->get_line_no(),
                         node->get_col_no());
  } else if (std::holds_alternative<ConstantNode *>(parse_result)) {
    auto node = std::get<ConstantNode *>(parse_result);
    throw ParseException("expected CondExpression but got Constant", node->get_line_no(),
                         node->get_col_no());
  } else {
    auto node = std::get<IdentifierNode *>(parse_result);
    throw ParseException("expected CondExpression but got Identifier", node->get_line_no(),
                         node->get_col_no());
  }

  std::vector<StatementNode *> stmt_lst = ParseStmtLst(lexer, state);

  return new WhileNode(cond, stmt_lst, stmt_no, {start_line, start_col});
}

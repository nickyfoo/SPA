#pragma once

#include "AST.hpp"
#include "Lexer.h"
#include <vector>

namespace parser {

class State {
public:
  int stmtCount;

  State() : stmtCount(0){};
};

ast::ProgramNode *parseProgram(lexer::BufferedLexer *lexer, State *state);
ast::ProcedureNode *parseProcedure(lexer::BufferedLexer *lexer, State *state);
ast::ReadNode *parseRead(lexer::BufferedLexer *lexer, State *state);
ast::PrintNode *parsePrint(lexer::BufferedLexer *lexer, State *state);
ast::CallNode *parseCall(lexer::BufferedLexer *lexer, State *state);
ast::WhileNode *parseWhile(lexer::BufferedLexer *lexer, State *state);
ast::IfNode *parseIf(lexer::BufferedLexer *lexer, State *state);
ast::AssignNode *parseAssign(lexer::BufferedLexer *lexer, State *state);
ast::ExpressionNode *parseExpression(lexer::BufferedLexer *lexer, State *state);
ast::CondExpressionNode *parseCondExpression(lexer::BufferedLexer *lexer, State *state);
ast::RelExpressionNode *parseRelExpression(lexer::BufferedLexer *lexer, State *state);

std::vector<ast::Node *> parseStmtLst(lexer::BufferedLexer *lexer, State *state);

} // namespace parser

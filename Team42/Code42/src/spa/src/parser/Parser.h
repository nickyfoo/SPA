#pragma once

#include "AST.hpp"
#include "Lexer.h"
#include <vector>

namespace parser {
ast::ProgramNode *parseProgram(lexer::BufferedLexer *lexer);
ast::ProcedureNode *parseProcedure(lexer::BufferedLexer *lexer);
ast::ReadNode *parseRead(lexer::BufferedLexer *lexer);
ast::PrintNode *parsePrint(lexer::BufferedLexer *lexer);
ast::CallNode *parseCall(lexer::BufferedLexer *lexer);
ast::WhileNode *parseWhile(lexer::BufferedLexer *lexer);
ast::IfNode *parseIf(lexer::BufferedLexer *lexer);
ast::AssignNode *parseAssign(lexer::BufferedLexer *lexer);
ast::ExpressionNode *parseExpression(lexer::BufferedLexer *lexer);
ast::CondExpressionNode *parseCondExpression(lexer::BufferedLexer *lexer);
ast::RelExpressionNode *parseRelExpression(lexer::BufferedLexer *lexer);

std::vector<ast::Node> *parseStmtLst(lexer::BufferedLexer *lexer);
} // namespace parser

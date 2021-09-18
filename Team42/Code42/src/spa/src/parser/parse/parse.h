#pragma once

#include <map>
#include <vector>
#include <string>

#include "ast.h"
#include "lexer.h"

enum class IdentifierType { ProcedureType, VariableType };

class ParseState {
 public:
  int stmt_count_;
};

class ParseException : public std::exception {
 public:
  ParseException(std::string error, int line_no, int col_no);
  virtual const char *what() const noexcept;

 private:
  std::string msg_;
};

ProgramNode *ParseProgram(BufferedLexer *lexer, ParseState *state);

ProcedureNode *ParseProcedure(BufferedLexer *lexer, ParseState *state);

ReadNode *ParseRead(BufferedLexer *lexer, ParseState *state);

PrintNode *ParsePrint(BufferedLexer *lexer, ParseState *state);

CallNode *ParseCall(BufferedLexer *lexer, ParseState *state);

WhileNode *ParseWhile(BufferedLexer *lexer, ParseState *state);

IfNode *ParseIf(BufferedLexer *lexer, ParseState *state);

AssignNode *ParseAssign(BufferedLexer *lexer, ParseState *state);

Node *ParseExpression(BufferedLexer *lexer, ParseState *state);

Node *ParseCondExpression(BufferedLexer *lexer, ParseState *state);

RelExpressionNode *ParseRelExpression(BufferedLexer *lexer, ParseState *state);

std::vector<StatementNode *> ParseStmtLst(BufferedLexer *lexer, ParseState *state);


#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

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

std::variant<CondExpressionNode *, RelExpressionNode *, ExpressionNode *, ConstantNode *,
             IdentifierNode *>
ParseExpression(BufferedLexer *lexer, ParseState *state, std::string end);

std::vector<StatementNode *> ParseStmtLst(BufferedLexer *lexer, ParseState *state);

// util functions
bool IsExprOp(TokenType t);
bool IsCondExprOp(TokenType t);
bool IsRelExprOp(TokenType t);
ExprOp ExprOpFromToken(const Token *t);
RelExprOp RelExprOpFromToken(const Token *t);
CondExprOp CondExprOpFromToken(const Token *t);
bool IsExpressionToken(const Token *t);
bool Precedes(TokenType op1, TokenType op2);
ConstantNode *MakeConstantNodeFromToken(const Token *t);
IdentifierNode *MakeIdentifierNodeFromToken(const Token *t);

#include "lexer.h"
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("Lexer 1st Test") {
  std::string source = "procedure main {\n"
                       "\tflag = 0;\n"
                       "\tcall computeCentroid;\n"
                       "\tcall printResults;\n"
                       "}";

  int numExpectedTokens = 14;
  const Token *expectedTokens[] = {
      new Token(TokenType::Procedure, "", 1, 0),
      new Token(TokenType::Identifier, "main", 1, 10),
      new Token(TokenType::LBrace, "", 1, 15),
      new Token(TokenType::Identifier, "flag", 2, 1),
      new Token(TokenType::Equal, "", 2, 6),
      new Token(TokenType::Constant, "0", 2, 8),
      new Token(TokenType::Semicolon, "", 2, 9),
      new Token(TokenType::Call, "", 3, 1),
      new Token(TokenType::Identifier, "computeCentroid", 3, 6),
      new Token(TokenType::Semicolon, "", 3, 21),
      new Token(TokenType::Call, "", 4, 1),
      new Token(TokenType::Identifier, "printResults", 4, 6),
      new Token(TokenType::Semicolon, "", 4, 18),
      new Token(TokenType::RBrace, "", 5, 0),
  };

  Lexer L(source.c_str());
  for (int i = 0; i < numExpectedTokens; i++) {
    const Token *t = L.getNextToken();
    const Token *e = expectedTokens[i];
    REQUIRE(t->kind == e->kind);
    REQUIRE(t->value == e->value);
    REQUIRE(t->lineNo == e->lineNo);
    REQUIRE(t->colNo == e->colNo);
  }

  BufferedLexer B(source.c_str());
  for (int i = 0; i < numExpectedTokens; i++) {
    if (i % 2 == 0) {
      const Token *t = B.peekNextToken();
      const Token *e = expectedTokens[i];
      REQUIRE(t->kind == e->kind);
      REQUIRE(t->value == e->value);
      REQUIRE(t->lineNo == e->lineNo);
      REQUIRE(t->colNo == e->colNo);
      B.getNextToken();
    } else {
      const Token *t = B.getNextToken();
      const Token *e = expectedTokens[i];
      REQUIRE(t->kind == e->kind);
      REQUIRE(t->value == e->value);
      REQUIRE(t->lineNo == e->lineNo);
      REQUIRE(t->colNo == e->colNo);
    }
  }
}

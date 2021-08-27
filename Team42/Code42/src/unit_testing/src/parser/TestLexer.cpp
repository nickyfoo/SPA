#include "Lexer.h"
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;
using namespace lexer;

TEST_CASE("Lexer 1st Test") {
  std::string source = "procedure main {\n"
                       "\tflag = 0;\n"
                       "\tcall computeCentroid;\n"
                       "\tcall printResults;\n"
                       "}";
  Lexer L(source.c_str());

  int numExpectedTokens = 14;

  Token *expectedTokens[] = {
      new Token(Kind::Procedure, "", 1, 0),
      new Token(Kind::Identifier, "main", 1, 10),
      new Token(Kind::LBrace, "", 1, 15),
      new Token(Kind::Identifier, "flag", 2, 1),
      new Token(Kind::Equal, "", 2, 6),
      new Token(Kind::Constant, "0", 2, 8),
      new Token(Kind::Semicolon, "", 2, 9),
      new Token(Kind::Call, "", 3, 1),
      new Token(Kind::Identifier, "computeCentroid", 3, 6),
      new Token(Kind::Semicolon, "", 3, 21),
      new Token(Kind::Call, "", 4, 1),
      new Token(Kind::Identifier, "printResults", 4, 6),
      new Token(Kind::Semicolon, "", 4, 18),
      new Token(Kind::RBrace, "", 5, 0),
  };

  for (int i = 0; i < numExpectedTokens; i++) {
    Token *t = L.getNextToken();
    Token *e = expectedTokens[i];
    REQUIRE(t->kind == e->kind);
    REQUIRE(t->value == e->value);
    REQUIRE(t->lineNo == e->lineNo);
    REQUIRE(t->colNo == e->colNo);
  }
}

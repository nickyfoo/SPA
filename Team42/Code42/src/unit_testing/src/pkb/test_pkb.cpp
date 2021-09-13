#include "parse.h"
#include <vector>
#include "pkb.h"
#include "catch.hpp"
#include "entities/statement.h"

std::string source = "procedure main {\n"
                     "\tflag = 0;\n"
                     "\tcall computeCentroid;\n"
                     "\tcall printResults;\n"
                     "}\n"
                     "procedure readPoint {\n"
                     "\tread x;\n"
                     "\tread y;\n"
                     "}\n"
                     "procedure printResults {\n"
                     "\tprint flag;\n"
                     "\tprint cenX;\n"
                     "\tprint cenY;\n"
                     "\tprint normSq;\n"
                     "}\n"
                     "procedure computeCentroid {\n"
                     "\tcount = 0;\n"
                     "\tcenX = 0;\n"
                     "\tcenY = 0;\n"
                     "\tcall readPoint;\n"
                     "\twhile((x != 0) && (y != 0)) {\n"
                     "\t\tcount = count+1;\n"
                     "\t\tcenX = cenX + x;\n"
                     "\t\tcenY = cenY + y;\n"
                     "\t\tcall readPoint;\n"
                     "\t}\n"
                     "\tif (count == 0) then {\n"
                     "\t\tflag = 1;\n"
                     "\t} else {\n"
                     "\t\tcenX = cenX / count;\n"
                     "\t\tcenY = cenY / count;\n"
                     "\t}\n"
                     "\tnormSq = cenX * cenX + cenY * cenY;\n"
                     "}";

TEST_CASE("Test pkb::Initialise()") {

  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
}

TEST_CASE("Test pkb::GetFollows()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);

  PKB pkb = PKB(p);
  pkb.PrintStatements();
  for (int i = 1; i <= pkb.get_num_statements() + 1; i++) {
    Statement *stmt = pkb.get_statement(i);
    // checking for NULL response
    if (!stmt) continue;
    stmt->FollowsInfo();
  }
}

TEST_CASE("Test pkb::GetParent()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  for (int i = 1; i <= pkb.get_num_statements() + 1; i++) {
    Statement *stmt = pkb.get_statement(i);
    // checking for NULL response
    if (!stmt) continue;
    stmt->ParentInfo();
  }
}
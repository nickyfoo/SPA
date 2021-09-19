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

TEST_CASE("Test PKB::Initialise()") {
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
  for (int i = 1; i <= pkb.get_num_statements(); i++) {
    Statement *stmt = pkb.get_statement(i);
    stmt->FollowsInfo();
  }
}

TEST_CASE("Test pkb::GetParent()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  for (int i = 1; i <= pkb.get_num_statements(); i++) {
    Statement *stmt = pkb.get_statement(i);
    stmt->ParentInfo();
  }
}

TEST_CASE("Test pkb::ExtractCalls()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintProcedures();
  std::vector<Procedure *> procedures = pkb.get_all_procedures();
  for (auto&proc:procedures){
    proc->CallsInfo();
  }
}

TEST_CASE("Test pkb::ExtractUsesModifies()") {
  BufferedLexer lexer(source.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  pkb.PrintProcedures();
  pkb.PrintVariables();
  std::vector<Statement *> statements = pkb.get_all_statements();
  for (auto &stmt : statements) {
    stmt->UsesInfo();
    stmt->ModifiesInfo();
  }
  std::vector<Procedure *> procedures = pkb.get_all_procedures();
  for (auto &proc : procedures) {
    proc->UsesInfo();
    proc->ModifiesInfo();
  }
  std::vector<Variable *> variables = pkb.get_all_variables();
  for (auto &var : variables) {
    var->UsesInfo();
    var->ModifiesInfo();
  }
}

TEST_CASE("Test pkb::ExtractUsesModifies() for containerstmts") {
  std::string source2 =
      "procedure main {"
      "if((x==0) && (y==0) ) then {"
      "a = b + c;"
      "} else {"
      "d = e + f;}"
      "}";

  BufferedLexer lexer(source2.c_str());
  ParseState s{};
  ProgramNode *p = ParseProgram(&lexer, &s);
  PKB pkb = PKB(p);
  pkb.PrintStatements();
  pkb.PrintProcedures();
  pkb.PrintVariables();
  std::vector<Statement *> statements = pkb.get_all_statements();
  for (auto &stmt : statements) {
    stmt->UsesInfo();
    stmt->ModifiesInfo();
  }
  std::vector<Procedure *> procedures = pkb.get_all_procedures();
  for (auto &proc : procedures) {
    proc->UsesInfo();
    proc->ModifiesInfo();
  }
  std::vector<Variable *> variables = pkb.get_all_variables();
  for (auto &var : variables) {
    var->UsesInfo();
    var->ModifiesInfo();
  }
}

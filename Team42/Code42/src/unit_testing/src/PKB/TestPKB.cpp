#include "Parser.h"
#include <vector>
#include "PKB.h"
#include "catch.hpp"
#include "Entities/Statement.h"

using namespace lexer;
using namespace parser;
using namespace ast;

std::string source = "procedure main {"
"flag = 0;"
"call computeCentroid;"
"call printResults;"
"}"
"procedure readPoint {"
"read x;"
"read y;"
"}"
"procedure printResults {"
"print flag;"
"print cenX;"
"print cenY;"
"print normSq;"
"}"
"procedure computeCentroid {"
"count = 0;"
"cenX = 0;"
"cenY = 0;"
"call readPoint;"
"while((x != 0) && (y != 0)) {"
"count = count+1;"
"cenX = cenX + x;"
"cenY = cenY + y;"
"call readPoint;"
"}"
"if (count == 0) then {"
"flag = 1;"
"} else {"
"cenX = cenX / count;"
"cenY = cenY / count;"
"}"
"normSq = cenX * cenX + cenY * cenY;"
"}";

TEST_CASE("Test PKB::Initialise()") {

    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);
    PKB pkb = PKB(p);
    pkb.Initialise();
}


TEST_CASE("Test PKB::GetFollows()") {
    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);

    PKB pkb = PKB(p);
    pkb.Initialise();
    pkb.PrintStatements();
    for (int i = 1; i <= pkb.GetNumStatements() + 1; i++) {
      Statement* s = pkb.GetStatement(i);
        // checking for NULL response
        if (!s) continue;
        s->FollowsInfo();
    }
}


TEST_CASE("Test PKB::GetParent()") {
    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);
    PKB pkb = PKB(p);
    pkb.Initialise();
    pkb.PrintStatements();
    for (int i = 1; i <= pkb.GetNumStatements() + 1; i++) {
      Statement* s = pkb.GetStatement(i);
        // checking for NULL response
        if (!s) continue;
        s->ParentInfo();
    }
}
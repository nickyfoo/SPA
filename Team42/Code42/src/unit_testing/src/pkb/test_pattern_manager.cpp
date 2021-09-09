#include "Parser.h"
#include <vector>
#include "pkb.h"
#include "catch.hpp"
#include "entities/statement.h"

using namespace std;
using namespace lexer;
using namespace parser;
using namespace ast;

TEST_CASE("PatternManager 1st Test") {
    Statement s1(1, ast::Assign);
    s1.SetExprString("cenX cenX * cenY cenY * +");

    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX + cenY * cenY", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX + cenY * cenY", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenY * cenY", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenY * cenY", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX + cenY", true) == false);

    Statement s2(1, ast::Assign);
    s2.SetExprString("count 1 +");
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count 1 +", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count", true) == true);

}

TEST_CASE("PatternManager 2nd Test") {
    // Which of the patterns match this assignment statement?
    // x = v + x * y + z * t
    Statement s1(1, ast::Assign);
    s1.SetExprString("v x y * + z t * +");

    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x * y + z * t", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "x * y", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x * y", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "y + z * t", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "x * y + z * t", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x * y + z * t", true) == true);
}

TEST_CASE("Test PatternManager TestAssignmentPattern") {
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

    // Parse source code
    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);
    PKB pkb = PKB(p);
    pkb.Initialise();
    vector<Statement*> assignment_stmts = pkb.GetStatements(ast::Assign);
    std::string pattern = "cenX * cenX + cenY * cenY";
    std::cout << "---------- PatternManager TestAssignmentPattern" << "\n";
    std::cout << "checking for " << pattern << "\n";
    for (Statement* stmt : assignment_stmts) {
      bool matchesPattern = PKB::TestAssignmentPattern(stmt, pattern, false);

        std::string exprString = stmt->GetExprString();
        if (matchesPattern) {
            std::cout << exprString;
            std::cout << " matches \n";
        } else {
            std::cout << exprString;
            std::cout << " does not match \n";
        }
    }
}
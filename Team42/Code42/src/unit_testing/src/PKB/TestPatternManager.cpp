#include "Parser.h"
#include <vector>
#include "PKB.h"
#include "PatternManager.h"
#include "catch.hpp"
#include "Entities/Statement.h"

using namespace std;
using namespace lexer;
using namespace parser;
using namespace ast;

TEST_CASE("PatternManager 1st Test") {
    Statement s1(1, ast::Assign);
    s1.setExprString("cenX cenX * cenY cenY * +");

    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenX * cenX + cenY * cenY", false) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenX * cenX + cenY * cenY", true) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenX * cenX", true) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenY * cenY", true) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenX * cenX", false) == false);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenY * cenY", false) == false);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "cenX + cenY", true) == false);

    Statement s2(1, ast::Assign);
    s2.setExprString("count 1 +");
    REQUIRE(PatternManager::checkAssignmentRhs(&s2, "count 1 +", false) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s2, "count", true) == true);

}

TEST_CASE("PatternManager 2nd Test") {
    // Which of the patterns match this assignment statement?
    // x = v + x * y + z * t
    Statement s1(1, ast::Assign);
    s1.setExprString("v x y * + z t * +");

    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "v + x * y + z * t", false) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "v", false) == false);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "v", true) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "x * y", true) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "v + x", true) == false);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "v + x * y", true) == true);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "y + z * t", true) == false);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "x * y + z * t", true) == false);
    REQUIRE(PatternManager::checkAssignmentRhs(&s1, "v + x * y + z * t", true) == true);
}

TEST_CASE("Test PatternManager checkAssignmentRhs") {
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
    pkb.initialPass();
    vector<Statement*> assignmentStmts = pkb.getStatements(ast::Assign);
    std::string pattern = "cenX * cenX + cenY * cenY";
    std::cout << "---------- PatternManager checkAssignmentRhs" << "\n";
    std::cout << "checking for " << pattern << "\n";
    for (Statement* stmt : assignmentStmts) {
        bool matchesPattern = PatternManager::checkAssignmentRhs(stmt, pattern, false);

        std::string exprString = stmt->getExprString();
        if (matchesPattern) {
            std::cout << exprString;
            std::cout << " matches \n";
        } else {
            std::cout << exprString;
            std::cout << " does not match \n";
        }
    }
}
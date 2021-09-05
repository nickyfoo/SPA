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
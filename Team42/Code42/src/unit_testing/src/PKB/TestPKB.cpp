#include "Parser.h"
#include <vector>
#include "PKB.h"
#include "catch.hpp"
#include "Entities/Statement.h"

using namespace std;
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

TEST_CASE("Test PKB::initialPass()") {

    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);
    PKB pkb = PKB(p);
    pkb.initialPass();
}


TEST_CASE("Test PKB::getFollows()") {
    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);
    PKB pkb = PKB(p);
    pkb.initialPass();
    pkb.getFollows();
    cout << "done stuff\n";
    StmtTable::printStmts();
    for (int i = 1; i <= StmtTable::getLargestStmtNum()+1; i++) {
        Statement* s = StmtTable::getStatementByLineNo(i);
        // checking for NULL response
        if (!s) continue;
        s->FollowsInfo();
    }
}


TEST_CASE("Test PKB::getParent()") {
    BufferedLexer* B = new BufferedLexer(source.c_str());
    State* s = new State{};
    ProgramNode* p = parseProgram(B, s);
    PKB pkb = PKB(p);
    pkb.initialPass();
    pkb.getParent();
    StmtTable::printStmts();
    for (int i = 1; i <= StmtTable::getLargestStmtNum() + 1; i++) {
        Statement* s = StmtTable::getStatementByLineNo(i);
        // checking for NULL response
        if (!s) continue;
        s->ParentInfo();
    }
}
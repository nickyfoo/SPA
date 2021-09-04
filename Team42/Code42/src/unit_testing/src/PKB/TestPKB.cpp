#include "Parser.h"
#include <string>
#include <vector>
#include "PKB.h"
#include "catch.hpp"

using namespace std;
using namespace lexer;
using namespace parser;
using namespace ast;

TEST_CASE("Test PKB::initialPass()") {
    std::string source = "procedure main {\n"
        "\tflag = flag;\n"
        "\tflag2 = gg;\n"
        "\tflag3 = hh;\n"
        "}";

    BufferedLexer* B = new BufferedLexer(source.c_str());
    ProgramNode* p = parseProgram(B);
    cout << "program passed to us has kind: " << p->kind << '\n';
    PKB pkb = PKB(p);
    pkb.initialPass();
}


TEST_CASE("Test PKB::getFollows()") {
    std::string source = "procedure main {\n"
        "\tflag = flag;\n"
        "\tflag2 = gg;\n"
        "\tflag3 = hh;\n"
        "}"
        "procedure main2 {\n"
        "\tmainer = 2;\n"
        "\tmainer = mainer + 2;\n"
        "}";


    BufferedLexer* B = new BufferedLexer(source.c_str());
    ProgramNode* p = parseProgram(B);
    PKB pkb = PKB(p);
    pkb.initialPass();
    pkb.getFollows();
    cout << "done stuff\n";
    StmtTable::printStmts();
    for (int i = 1; i <= StmtTable::getLargestStmtNum()+1; i++) {
        Statement* s = StmtTable::getStmt(i);
        // checking for NULL response
        if (!s) continue;
        s->info();
    }
}
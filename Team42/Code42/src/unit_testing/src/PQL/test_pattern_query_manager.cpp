#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"

TEST_CASE("Test 1: Assign Pattern with Synonym and Wild Card") {
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
    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(v, _)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    if (clause == nullptr) {
        printf("Invalid PQL Query!\n");
    } else {
        for (std::string entity : *clause->get_query_entities()) {
            std::cout << "Select: " << entity << "\n";
        }
        std::cout << "\n" << std::flush;
        for (SuchThatClause* relationship : *clause->get_query_relationships()) {
            std::cout << "SuchThatClause type: "
            << relationship->get_type_str() << "\n";
            if (relationship->get_left_ref()->get_type()
            == SuchThatRefType::Statement) {
                std::cout << "Left ref: "
                << relationship->get_left_ref()->get_stmt_ref().get_value();
            } else if (relationship->get_left_ref()->get_type()
            == SuchThatRefType::Entity) {
                std::cout << "Left ref: "
                << relationship->get_left_ref()->get_ent_ref().get_value();
            }

            if (relationship->get_right_ref()->get_type()
            == SuchThatRefType::Statement) {
                std::cout << "Right ref: "
                << relationship->get_right_ref()->get_stmt_ref().get_value();
            } else if (relationship->get_right_ref()->get_type()
            == SuchThatRefType::Entity) {
                std::cout << "Right ref: "
                << relationship->get_right_ref()->get_ent_ref().get_value();
            }
        }
        std::cout << "\n" << std::flush;
        for (PatternClause* pattern : *clause->get_query_patterns()) {
            std::cout << "PatternClause synonym_: "
            << pattern->get_synonym()->get_synonym()
            << "\nIs partial pattern? "
            << (pattern->get_right_ref()->IsPartialPattern() ? "Yes" : "No")
            << "\nLeft ref: "
            << pattern->get_left_ref()->get_value()
            << " Right ref: "<< (pattern->get_right_ref()->IsWildCard()
            ? "_" : pattern->get_right_ref()->get_expression()) << "\n";
        }
    }
    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 2: Assign Pattern with Synonym and Argument") {
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

    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(v, '0')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "10", "11", "12"};
    printf("WAT THE RESULT\n");
    for (int j = 0; j < ret->size(); j++) {
        printf("%s\n",ret->at(j).c_str());
    }
    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 3: Assign Pattern with Synonym and Synonym") {
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
    std::string ss = "assign a; variable v1, v2; \n"
                     "Select a pattern a(v1, v2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"15", "16", "17", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 4: Assign Pattern with Synonym and Partial Pattern") {
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
    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(v, _'cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"16", "21", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 5: Assign Pattern with Wild Card and Wild Card") {
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
    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(_, _)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "10", "11", "12", "15", "16", "17", "20", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 6: Assign Pattern with Wild Card and Argument") {
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
    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(_, '0')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"1", "10", "11", "12"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 7: Assign Pattern with Wild Card and Synonym") {
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
    std::string ss = "assign a; variable v2; \n"
                     "Select a pattern a(_, v2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"15", "16", "17", "21", "22", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 8: Assign Pattern with Wild Card and Partial Pattern") {
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
    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(_, _'cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {"16", "21", "23"};

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 9: Non-Assign Pattern with Wild Card and Partial Pattern") {
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
    std::string ss = "while w; variable v;\n"
                     "Select w pattern w(_, _'cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {};

    REQUIRE(ret == nullptr);
}

TEST_CASE("Test 10: Assign Pattern with Wild Card and Partial Pattern") {
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
    std::string ss = "while w; variable v;\n"
                     "Select w pattern w(_, _'cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {};

    REQUIRE(ret == nullptr);
}

TEST_CASE("Test 10: Assign Pattern with Wild Card and Missing Partial Pattern") {
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
    std::string ss = "assign a; variable v;\n"
                     "Select a pattern a(_, 'cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {};

    REQUIRE(ret == nullptr);
}

TEST_CASE("Test 10: Assign Pattern with Non Variable and Partial Pattern") {
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
    std::string ss = "assign a; stmt s;\n"
                     "Select a pattern a(s, _'cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();

    // Parse source
    BufferedLexer lexer(source.c_str());
    ParseState s{};
    ProgramNode* p = ParseProgram(&lexer, &s);
    PKB pkb = PKB(p);

    auto evaluator = new QueryEvaluator(clause, &pkb);
    std::vector<std::string> *ret = evaluator->Evaluate();

    std::vector<std::string> expected = {};

    REQUIRE(ret == nullptr);
}
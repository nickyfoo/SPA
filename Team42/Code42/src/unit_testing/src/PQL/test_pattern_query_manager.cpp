#include "query_evaluator.h"
#include "query_preprocessor.h"
#include "catch.hpp"

#include "parse.h"
#include <string>
#include <vector>
#include "pkb.h"

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

TEST_CASE("Test 1: Assign Pattern with Synonym and Wild Card") {
    std::string ss = "assign a; var v;\n"
                     "Select a pattern a(v, _)";
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

TEST_CASE("Test 2: Assign Pattern with Synonym and Argument") {
    std::string ss = "assign a; var v;\n"
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

    REQUIRE(ret->size() == expected.size());
    for (int i = 0; i < expected.size(); i++) {
        REQUIRE(ret->at(i) == expected.at(i));
    }
}

TEST_CASE("Test 3: Assign Pattern with Synonym and Synonym") {
    std::string ss = "assign a; var v1, v2; \n"
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
    std::string ss = "assign a; var v;\n"
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
    std::string ss = "assign a; var v;\n"
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
    std::string ss = "assign a; var v;\n"
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
    std::string ss = "assign a; var v2; \n"
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
    std::string ss = "assign a; var v;\n"
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
    std::string ss = "while w; var v;\n"
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
    std::string ss = "while w; var v;\n"
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
    std::string ss = "assign a; var v;\n"
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
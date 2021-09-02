#include "QueryObject.h"
#include "PQLQuery.h"
#include "catch.hpp"

using namespace std;

TEST_CASE("1. Standard select") {
    string ss = "stmt s;\n"
                "Select s";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("2. No entity found") {
    string ss = "stmt s;\n"
                "Select s1";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("3. Lowercase select") {
    string ss = "stmt s;\n"
                "select s";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("4. Empty select") {
    string ss = "stmt s;\n"
                "Select";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("5. Additional spaces in select statement") {
    string ss = "stmt s;\n"
                "Select    s ";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("6. Wrong such that keyword") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 st Follows(s1, s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("7. Invalid relationship") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Following(s1, s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("8. Follows with string argument") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows('test', s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("9. Follows clause with 2 statements") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows(s1, s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("10. Follows clause with 1 statement and 1 statement number") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows(s1, 2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("11. Follows clause with 1 statement number and 1 statement") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows(1, s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("12. Follows clause with 1 statement and invalid entity") {
    string ss = "stmt s1; stmt s2; var v;\n"
                "Select s1 such that Follows(s1, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("13. Follows* clause with 2 statements") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows*(s1,s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}


TEST_CASE("14. Follows* clause with 1 statement and 1 statement number") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows*(s1,2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}


TEST_CASE("15. Follows* clause with additional spaces") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows*(s1  , s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("16. Parent with string argument") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent('test', s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("17. Parent clause with 2 statements") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent(s1, s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("18. Parent clause with 1 statement and 1 statement number") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent(s1, 2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("19. Parent clause with 1 statement number and 1 statement") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent(1, s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("20. Parent clause with 1 statement and invalid entity") {
    string ss = "stmt s1; stmt s2; var v;\n"
                "Select s1 such that Parent(s1, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("21. Parent* clause with 2 statements") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1,s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}


TEST_CASE("22. Parent* clause with 1 statement and 1 statement number") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1,2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}


TEST_CASE("23. Follows* clause with additional spaces") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1  , s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}


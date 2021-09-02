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


TEST_CASE("23. Parent* clause with additional spaces") {
    string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1  , s2)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("24. Modifies clause with assignment and variable") {
    string ss = "assign a; variable v;\n"
                "Select a such that Modifies(a, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("25. Modifies clause with print and variable") {
    string ss = "print pn; variable v;\n"
                "Select pn such that Modifies(pn, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("26. Modifies clause with if and variable") {
    string ss = "if ifs; variable v;\n"
                "Select ifs such that Modifies(ifs, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("27. Modifies clause with while and variable") {
    string ss = "while whiles; variable v;\n"
                "Select whiles such that Modifies(whiles, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("28. Modifies clause with procedure and variable") {
    string ss = "procedure proc; variable v;\n"
                "Select proc such that Modifies(proc, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("29. Modifies clause with call and variable") {
    string ss = "call calls; variable v;\n"
                "Select calls such that Modifies(calls, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("30. Modifies clause with statement and variable") {
    string ss = "stmt s; variable v;\n"
                "Select s such that Modifies(s, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("31. Modifies clause with statement number and variable") {
    string ss = "variable v;\n"
                "Select v such that Modifies(2, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("32. Modifies clause with assignment and statement") {
    string ss = "assign assign; stmt s;\n"
                "Select assign such that Modifies(assign, s)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("33. Modifies clause with assignment and statement number") {
    string ss = "assign assign; stmt s;\n"
                "Select assign such that Modifies(pn, 50)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("34. Uses clause with assignment and variable") {
    string ss = "assign a; variable v;\n"
                "Select a such that Uses(a, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("35. Uses clause with print and variable") {
    string ss = "print pn; variable v;\n"
                "Select pn such that Uses(pn, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("36. Uses clause with if and variable") {
    string ss = "if ifs; variable v;\n"
                "Select ifs such that Uses(ifs, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("37. Uses clause with while and variable") {
    string ss = "while whiles; variable v;\n"
                "Select whiles such that Uses(whiles, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("38. Uses clause with procedure and variable") {
    string ss = "procedure proc; variable v;\n"
                "Select proc such that Uses(proc, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("39. Uses clause with call and variable") {
    string ss = "call calls; variable v;\n"
                "Select calls such that Uses(calls, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("40. Uses clause with statement and variable") {
    string ss = "stmt s; variable v;\n"
                "Select s such that Uses(s, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("41. Uses clause with statement number and variable") {
    string ss = "variable v;\n"
                "Select v such that Uses(2, v)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause != nullptr);
}

TEST_CASE("42. Uses clause with assignment and statement") {
    string ss = "assign assign; stmt s;\n"
                "Select assign such that Uses(assign, s)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}

TEST_CASE("43. Uses clause with assignment and statement number") {
    string ss = "assign assign; stmt s;\n"
                "Select assign such that Uses(pn, 50)";
    QueryObject* query = new QueryObject(ss);
    PQLQuery *clause = query->getPQLQuery();
    REQUIRE(clause == nullptr);
}



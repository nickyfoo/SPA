#include "query_preprocessor.h"
#include "pql_query.h"
#include "catch.hpp"

TEST_CASE("1. Standard select") {
    std::string ss = "stmt s;\n"
                "Select s";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("2. No entity found") {
    std::string ss = "stmt s;\n"
                "Select s1";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("3. Lowercase select") {
    std::string ss = "stmt s;\n"
                "select s";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("4. Empty select") {
    std::string ss = "stmt s;\n"
                "Select";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("5. Additional spaces in select statement") {
    std::string ss = "stmt s;\n"
                "Select    s ";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("6. Wrong such that keyword") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 st Follows(s1, s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("7. Invalid relationship") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Following(s1, s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("8. Follows with string argument_") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows('test', s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("9. Follows clause with 2 statements") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows(s1, s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("10. Follows clause with 1 statement and 1 statement number") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows(s1, 2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("11. Follows clause with 1 statement number and 1 statement") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows(1, s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("12. Follows clause with 1 statement and invalid entity") {
    std::string ss = "stmt s1; stmt s2; var v;\n"
                "Select s1 such that Follows(s1, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("13. Follows* clause with 2 statements") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows*(s1,s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}


TEST_CASE("14. Follows* clause with 1 statement and 1 statement number") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows*(s1,2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}


TEST_CASE("15. Follows* clause with additional spaces") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Follows*(s1  , s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("16. Parent with string argument_") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent('test', s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("17. Parent clause with 2 statements") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent(s1, s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("18. Parent clause with 1 statement and 1 statement number") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent(s1, 2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("19. Parent clause with 1 statement number and 1 statement") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent(1, s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("20. Parent clause with 1 statement and invalid entity") {
    std::string ss = "stmt s1; stmt s2; var v;\n"
                "Select s1 such that Parent(s1, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("21. Parent* clause with 2 statements") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1,s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}


TEST_CASE("22. Parent* clause with 1 statement and 1 statement number") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1,2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}


TEST_CASE("23. Parent* clause with additional spaces") {
    std::string ss = "stmt s1; stmt s2;\n"
                "Select s1 such that Parent*(s1  , s2)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("24. Modifies clause with assignment and variable") {
    std::string ss = "assign a; variable v;\n"
                "Select a such that Modifies(a, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("25. Modifies clause with print and variable") {
    std::string ss = "print pn; variable v;\n"
                "Select pn such that Modifies(pn, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("26. Modifies clause with if and variable") {
    std::string ss = "if ifs; variable v;\n"
                "Select ifs such that Modifies(ifs, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("27. Modifies clause with while and variable") {
    std::string ss = "while whiles; variable v;\n"
                "Select whiles such that Modifies(whiles, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("28. Modifies clause with procedure and variable") {
    std::string ss = "procedure proc; variable v;\n"
                "Select proc such that Modifies(proc, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("29. Modifies clause with call and variable") {
    std::string ss = "call calls; variable v;\n"
                "Select calls such that Modifies(calls, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("30. Modifies clause with statement and variable") {
    std::string ss = "stmt s; variable v;\n"
                "Select s such that Modifies(s, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("31. Modifies clause with statement number and variable") {
    std::string ss = "variable v;\n"
                "Select v such that Modifies(2, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("32. Modifies clause with assignment and statement") {
    std::string ss = "assign assign; stmt s;\n"
                "Select assign such that Modifies(assign, s)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("33. Modifies clause with unknown variable") {
    std::string ss = "assign assign; stmt s;\n"
                "Select assign such that Modifies(pn, 50)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("34. Uses clause with assignment and variable") {
    std::string ss = "assign a; variable v;\n"
                "Select a such that Uses(a, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("35. Uses clause with print and variable") {
    std::string ss = "print pn; variable v;\n"
                "Select pn such that Uses(pn, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("36. Uses clause with if and variable") {
    std::string ss = "if ifs; variable v;\n"
                "Select ifs such that Uses(ifs, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("37. Uses clause with while and variable") {
    std::string ss = "while whiles; variable v;\n"
                "Select whiles such that Uses(whiles, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("38. Uses clause with procedure and variable") {
    std::string ss = "procedure proc; variable v;\n"
                "Select proc such that Uses(proc, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("39. Uses clause with call and variable") {
    std::string ss = "call calls; variable v;\n"
                "Select calls such that Uses(calls, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("40. Uses clause with statement and variable") {
    std::string ss = "stmt s; variable v;\n"
                "Select s such that Uses(s, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("41. Uses clause with statement number and variable") {
    std::string ss = "variable v;\n"
                "Select v such that Uses(2, v)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("42. Uses clause with assignment and statement") {
    std::string ss = "assign assign; stmt s;\n"
                "Select assign such that Uses(assign, s)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("43. Uses clause with assignment and statement number") {
    std::string ss = "assign assign; stmt s;\n"
                "Select assign such that Uses(pn, 50)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("44. Standard pattern clause") {
    std::string ss = "assign a;\n"
                     "Select a pattern a ( _ , 'count + 1')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("45. PatternClause clause with partial pattern") {
    std::string ss = "assign a;\n"
                     "Select a pattern a ( 'normSq' , _'cenX * cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("46. PatternClause clause with different variable name") {
    std::string ss = "assign newa;\n"
                     "Select newa pattern newa( 'normSq' , _'cenX * cenX'_)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("47. Queries with one pattern clause and one such that") {
    std::string ss = "assign a; while w;"
                     "Select w such that Parent* (w, a) pattern a ('count', _)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("48. Queries with one pattern clause and one such that") {
    std::string ss = "assign a; variable v;"
                     "Select a such that Uses (a, v) pattern a (v, _)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("49. Queries that use and modify the same variable") {
    std::string ss = "assign a; while w;"
                     "Select a pattern a ('x', _) such that Uses (a, 'x')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("50. Complex queries with keywords as variable name") {
    std::string ss = "assign pattern; while w;"
                     "Select pattern pattern pattern ('x', _) "
                     "such that Uses (pattern, 'x')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("51. Complex queries with keywords as variable name") {
    std::string ss = "assign pattern; variable select;"
                     "Select pattern pattern pattern (select, _) "
                     "such that Uses (pattern, select)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("52. Extra such that") {
    std::string ss = "assign pattern; variable select;"
                     "Select pattern pattern pattern (select, _) "
                     "such that such that Uses (pattern, select)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("53. Strange such that") {
    std::string ss = "assign pattern; variable select;"
                     "Select pattern pattern pattern (select, _) "
                     "such that (,)    such that Uses (pattern, select)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("54. Bad Pattern ending with symbol") {
    std::string ss = "assign a;"
                     "Select a pattern a ( _ , 'x+y*')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("55. Bad Pattern with two symbols") {
    std::string ss = "assign a;"
                     "Select a pattern a ( _ , 'x+*y')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}

TEST_CASE("56. Pattern keyword inside pattern") {
    std::string ss = "assign a;"
                     "Select a pattern a ( _ , ' pattern ')";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause != nullptr);
}

TEST_CASE("57. Weird select clauses_") {
    std::string ss = "stmt s;"
                     "Select s hello Follows(s, 4)";
    auto* query = new QueryPreprocessor(ss);
    PQLQuery *clause = query->get_pql_query();
    REQUIRE(clause == nullptr);
}


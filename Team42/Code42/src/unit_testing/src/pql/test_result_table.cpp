#include "result_table.h"
#include "catch.hpp"

TEST_CASE("AddColumns") {
  ResultTable result_table = ResultTable();
  table *expected = new std::vector<std::vector<std::string>>();
  SECTION("AddColumns_SingleColumn_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    expected->push_back({"one", "four", "seven"});
    expected->push_back({"two", "five", "eight"});
    expected->push_back({"three", "six", "nine"});
    REQUIRE(result_table.get_synonym_to_index()->size() == 3);
    REQUIRE(result_table.get_table()->at(0) == expected->at(0));
    REQUIRE(result_table.get_table()->at(1) == expected->at(1));
    REQUIRE(result_table.get_table()->at(2) == expected->at(2));
    REQUIRE(result_table.get_synonym_to_index()->at("a") == 0);
    REQUIRE(result_table.get_synonym_to_index()->at("b") == 1);
    REQUIRE(result_table.get_synonym_to_index()->at("c") == 2);
  }

  SECTION("AddColumns_DoubleColumns_ReturnsExpected") {
    result_table.AddDoubleColumns("a", {"one", "two", "three"}, "b", {"four", "five", "six"});
    result_table.AddDoubleColumns("c", {"seven", "eight", "nine"}, "d", {"ten", "eleven", "twelve"});
    expected->push_back({"one", "four", "seven", "ten"});
    expected->push_back({"two", "five", "eight", "eleven"});
    expected->push_back({"three", "six", "nine", "twelve"});
    REQUIRE(result_table.get_synonym_to_index()->size() == 4);
    REQUIRE(result_table.get_table()->at(0) == expected->at(0));
    REQUIRE(result_table.get_table()->at(1) == expected->at(1));
    REQUIRE(result_table.get_table()->at(2) == expected->at(2));
    REQUIRE(result_table.get_synonym_to_index()->at("a") == 0);
    REQUIRE(result_table.get_synonym_to_index()->at("b") == 1);
    REQUIRE(result_table.get_synonym_to_index()->at("c") == 2);
    REQUIRE(result_table.get_synonym_to_index()->at("d") == 3);
  }

  SECTION("AddColumns_SingleAndDoubleColumns_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddDoubleColumns("c", {"seven", "eight", "nine"}, "d", {"ten", "eleven", "twelve"});
    expected->push_back({"one", "four", "seven", "ten"});
    expected->push_back({"two", "five", "eight", "eleven"});
    expected->push_back({"three", "six", "nine", "twelve"});
    REQUIRE(result_table.get_synonym_to_index()->size() == 4);
    REQUIRE(result_table.get_table()->at(0) == expected->at(0));
    REQUIRE(result_table.get_table()->at(1) == expected->at(1));
    REQUIRE(result_table.get_table()->at(2) == expected->at(2));
    REQUIRE(result_table.get_synonym_to_index()->at("a") == 0);
    REQUIRE(result_table.get_synonym_to_index()->at("b") == 1);
    REQUIRE(result_table.get_synonym_to_index()->at("c") == 2);
    REQUIRE(result_table.get_synonym_to_index()->at("d") == 3);
  }

  SECTION("AddColumns_DifferentSize_ThrowsException") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    REQUIRE_THROWS_AS(    result_table.AddSingleColumn("b", {"four", "five", "six", "seven"}), std::runtime_error);
  }
}

TEST_CASE("GetColumnVec") {
  ResultTable result_table = ResultTable();
  SECTION("GetColumnVec_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    std::vector<std::string> column_a = {"one", "two", "three"};
    std::vector<std::string> column_b = {"four", "five", "six"};
    std::vector<std::string> column_c = {"seven", "eight", "nine"};
    REQUIRE(column_a == result_table.GetColumnVec("a"));
    REQUIRE(column_b == result_table.GetColumnVec("b"));
    REQUIRE(column_c == result_table.GetColumnVec("c"));
  }

  SECTION("GetColumnVec_ThrowsException") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    std::vector<std::string> column_a = {"one", "two", "three"};
    std::vector<std::string> column_b = {"four", "five", "six"};
    std::vector<std::string> column_c = {"seven", "eight", "nine"};
    REQUIRE_THROWS_AS(result_table.GetColumnVec("d"), std::runtime_error);
  }
}

TEST_CASE("GetCommonSynonyms") {
  ResultTable result_table = ResultTable();
  ResultTable other_result_table = ResultTable();

  SECTION("GetCommonSynonyms_GetCommmonSynonyms0_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    other_result_table.AddSingleColumn("d", {"one", "two", "three"});
    other_result_table.AddSingleColumn("e", {"four", "five", "six"});
    other_result_table.AddSingleColumn("f", {"seven", "eight", "nine"});
    std::vector<std::pair<std::string, int>> *result = result_table.GetCommonSynonyms(other_result_table);
    std::vector<std::pair<std::string, int>> *expected = new std::vector<std::pair<std::string, int>>();
    REQUIRE(result->size() == expected->size());
  }

  SECTION("GetCommonSynonyms_GetCommmonSynonyms1_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    other_result_table.AddSingleColumn("d", {"one", "two", "three"});
    other_result_table.AddSingleColumn("e", {"four", "five", "six"});
    other_result_table.AddSingleColumn("a", {"seven", "eight", "nine"});
    std::vector<std::pair<std::string, int>> *result = result_table.GetCommonSynonyms(other_result_table);
    std::vector<std::pair<std::string, int>> *expected = new std::vector<std::pair<std::string, int>>({std::make_pair("a", 2)});
    REQUIRE(result->size() == expected->size());
    REQUIRE(result->at(0).first == expected->at(0).first);
    REQUIRE(result->at(0).second == expected->at(0).second);
  }

  SECTION("GetCommonSynonyms_GetCommmonSynonyms2_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    other_result_table.AddSingleColumn("d", {"one", "two", "three"});
    other_result_table.AddSingleColumn("b", {"four", "five", "six"});
    other_result_table.AddSingleColumn("a", {"seven", "eight", "nine"});
    std::vector<std::pair<std::string, int>> *result = result_table.GetCommonSynonyms(other_result_table);
    std::vector<std::pair<std::string, int>> *expected = new std::vector<std::pair<std::string, int>>(
        {std::make_pair("a", 2), std::make_pair("b", 1)});
    REQUIRE(result->size() == expected->size());
    REQUIRE(result->at(0).first == expected->at(0).first);
    REQUIRE(result->at(0).second == expected->at(0).second);
    REQUIRE(result->at(1).first == expected->at(1).first);
    REQUIRE(result->at(1).second == expected->at(1).second);
  }
}

TEST_CASE("Joins") {
  ResultTable result_table = ResultTable();
  ResultTable other_result_table = ResultTable();

  SECTION("Joins_NaturalJoin_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    other_result_table.AddSingleColumn("d", {"one", "two", "three"});
    other_result_table.AddSingleColumn("a", {"one", "two", "four"});
    result_table.NaturalJoin(other_result_table);
    table *expected = new std::vector<std::vector<std::string>>();
    expected->push_back({"one", "four", "seven", "one"});
    expected->push_back({"two", "five", "eight", "two"});
    std::vector<std::string> col_a = {"one", "two"};
    std::vector<std::string> col_b = {"four", "five"};
    std::vector<std::string> col_c = {"seven", "eight"};
    std::vector<std::string> col_d = {"one", "two"};
    REQUIRE(result_table.get_table()->size() == expected->size());
    REQUIRE(result_table.get_synonym_to_index()->size() == 4);
    REQUIRE(result_table.get_synonym_to_index()->at("a") == 0);
    REQUIRE(result_table.get_synonym_to_index()->at("b") == 1);
    REQUIRE(result_table.get_synonym_to_index()->at("c") == 2);
    REQUIRE(result_table.get_synonym_to_index()->at("d") == 3);
    REQUIRE(result_table.GetColumnVec("a") == col_a);
    REQUIRE(result_table.GetColumnVec("b") == col_b);
    REQUIRE(result_table.GetColumnVec("c") == col_c);
    REQUIRE(result_table.GetColumnVec("d") == col_d);
  }

  SECTION("Joins_CrossJoin_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    other_result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    other_result_table.AddSingleColumn("d", {"ten", "eleven", "twelve"});
    result_table.CrossJoin(other_result_table);
    table *expected = new std::vector<std::vector<std::string>>();
    expected->push_back({"one", "four", "seven", "ten"});
    expected->push_back({"one", "four", "eight", "eleven"});
    expected->push_back({"one", "four", "nine", "twelve"});

    expected->push_back({"two", "five", "seven", "ten"});
    expected->push_back({"two", "five", "eight", "eleven"});
    expected->push_back({"two", "five", "nine", "twelve"});

    expected->push_back({"three", "six", "seven", "ten"});
    expected->push_back({"three", "six", "eight", "eleven"});
    expected->push_back({"three", "six", "nine", "twelve"});

    std::vector<std::string> col_a = {"one", "one", "one",
                                      "two", "two", "two",
                                      "three", "three", "three"};
    std::vector<std::string> col_b = {"four", "four", "four",
                                      "five", "five", "five",
                                      "six", "six", "six"};
    std::vector<std::string> col_c = {"seven", "eight", "nine",
                                      "seven", "eight", "nine",
                                      "seven", "eight", "nine"};
    std::vector<std::string> col_d = {"ten", "eleven", "twelve",
                                      "ten", "eleven", "twelve",
                                      "ten", "eleven", "twelve"};
    REQUIRE(result_table.get_table()->size() == expected->size());
    REQUIRE(result_table.get_synonym_to_index()->size() == 4);
    REQUIRE(result_table.get_synonym_to_index()->at("a") == 0);
    REQUIRE(result_table.get_synonym_to_index()->at("b") == 1);
    REQUIRE(result_table.get_synonym_to_index()->at("c") == 2);
    REQUIRE(result_table.get_synonym_to_index()->at("d") == 3);
    REQUIRE(result_table.GetColumnVec("a") == col_a);
    REQUIRE(result_table.GetColumnVec("b") == col_b);
    REQUIRE(result_table.GetColumnVec("c") == col_c);
    REQUIRE(result_table.GetColumnVec("d") == col_d);
  }

  SECTION("Joins_NaturalJoinOnNoCommmon_ReturnsExpected") {
    result_table.AddSingleColumn("a", {"one", "two", "three"});
    result_table.AddSingleColumn("b", {"four", "five", "six"});
    other_result_table.AddSingleColumn("c", {"seven", "eight", "nine"});
    other_result_table.AddSingleColumn("d", {"ten", "eleven", "twelve"});
    result_table.CrossJoin(other_result_table);
    table *expected = new std::vector<std::vector<std::string>>();
    expected->push_back({"one", "four", "seven", "ten"});
    expected->push_back({"one", "four", "eight", "eleven"});
    expected->push_back({"one", "four", "nine", "twelve"});

    expected->push_back({"two", "five", "seven", "ten"});
    expected->push_back({"two", "five", "eight", "eleven"});
    expected->push_back({"two", "five", "nine", "twelve"});

    expected->push_back({"three", "six", "seven", "ten"});
    expected->push_back({"three", "six", "eight", "eleven"});
    expected->push_back({"three", "six", "nine", "twelve"});

    std::vector<std::string> col_a = {"one", "one", "one",
                                      "two", "two", "two",
                                      "three", "three", "three"};
    std::vector<std::string> col_b = {"four", "four", "four",
                                      "five", "five", "five",
                                      "six", "six", "six"};
    std::vector<std::string> col_c = {"seven", "eight", "nine",
                                      "seven", "eight", "nine",
                                      "seven", "eight", "nine"};
    std::vector<std::string> col_d = {"ten", "eleven", "twelve",
                                      "ten", "eleven", "twelve",
                                      "ten", "eleven", "twelve"};
    REQUIRE(result_table.get_table()->size() == expected->size());
    REQUIRE(result_table.get_synonym_to_index()->size() == 4);
    REQUIRE(result_table.get_synonym_to_index()->at("a") == 0);
    REQUIRE(result_table.get_synonym_to_index()->at("b") == 1);
    REQUIRE(result_table.get_synonym_to_index()->at("c") == 2);
    REQUIRE(result_table.get_synonym_to_index()->at("d") == 3);
    REQUIRE(result_table.GetColumnVec("a") == col_a);
    REQUIRE(result_table.GetColumnVec("b") == col_b);
    REQUIRE(result_table.GetColumnVec("c") == col_c);
    REQUIRE(result_table.GetColumnVec("d") == col_d);
  }
}
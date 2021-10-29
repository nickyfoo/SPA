#pragma once

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <entity.hpp>

using table = std::vector<std::vector<std::string>>;

class ResultTable {
 public:
  ResultTable();
  void NaturalJoin(ResultTable &result_table);
  void CrossJoin(ResultTable &result_table,
                 std::unordered_set<std::string> used_synonyms);
  void AddSingleColumn(std::string synonym, std::vector<std::string> vec);
  void AddDoubleColumns(std::string synonym_one, std::vector<std::string> vec_one,
                        std::string synonym_two, std::vector<std::string> vec_two);
  std::vector<std::string> GetColumnVec(std::string synonym);
  std::vector<std::pair<std::string, int>> *GetCommonSynonyms(ResultTable &result_table);
  void set_table(ResultTable &result_table);
  table *get_table();
  std::unordered_map<std::string, int> *get_synonym_to_index();
  std::unordered_map<int, std::string> *get_index_to_synonym();
  std::vector<std::string> *get_synonyms();
 private:
  table *table_;
  std::vector<std::string> *synonyms_;
  std::unordered_map<std::string, int> *synonym_to_index_;
  std::unordered_map<int, std::string> *index_to_synonym_;
};


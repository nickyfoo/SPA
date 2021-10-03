#include <stdio.h>
#include <string>
#include <unordered_map>
#include <entity.hpp>

using table = std::vector<std::vector<std::string>>;

class ResultTable {
 public:
  ResultTable(std::vector<std::string> synonyms);
  ResultTable();
  void NaturalJoin(ResultTable &result_table);
  void CrossJoin(ResultTable &result_table);
  void AddSingleColumn(std::string synonym, std::vector<std::string> vec);
  void AddDoubleColumns(std::string synonym_one, std::vector<std::string> vec_one,
                        std::string synonym_two, std::vector<std::string> vec_two);
  void set_table(ResultTable &result_table);
  table* get_table();
  std::unordered_map<std::string, int> *get_synonym_to_index();
 private:
  table* table_;
  std::vector<std::string> synonyms_;
  std::unordered_map<std::string, int> *synonym_to_index_;
  std::vector<std::pair<std::string, int>> *GetCommonSynonyms(ResultTable &result_table);
};


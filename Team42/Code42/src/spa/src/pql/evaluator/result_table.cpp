#include "result_table.h"
#include <algorithm>
#include <stdexcept>

ResultTable::ResultTable() {
  this->table_ = new std::vector<std::vector<std::string>>();
  this->synonyms_ = new std::vector<std::string>();
  this->synonym_to_index_ = new std::unordered_map<std::string, int>();
  this->index_to_synonym_ = new std::unordered_map<int, std::string>();
}

void ResultTable::NaturalJoin(ResultTable &other_result_table) {
  auto common_synonyms = GetCommonSynonyms(other_result_table);
  table *other_table = other_result_table.get_table();
  if (other_table->empty()) {
    delete table_;
    table_ = new std::vector<std::vector<std::string>>();
    return;
  }

  if (common_synonyms->size() == 2) {  // two common
    std::vector<std::string> col_one = GetColumnVec(common_synonyms->at(0).first);
    std::vector<std::string> col_two = GetColumnVec(common_synonyms->at(1).first);
    std::vector<std::string> other_col_one = other_result_table.GetColumnVec(
        common_synonyms->at(0).first);
    std::vector<std::string> other_col_two = other_result_table.GetColumnVec(
        common_synonyms->at(1).first);

    std::vector<int> index_to_erase;
    for (int i = 0; i < col_one.size() && i < col_two.size(); ++i) {
      bool found_matching_row = false;
      for (int j = 0; j < other_col_one.size() && j < other_col_two.size(); ++j) {
        if (col_one.at(i) == other_col_one.at(j) && col_two.at(i) == other_col_two.at(j)) {
          found_matching_row = true;
          break;
        }
      }
      if (!found_matching_row) {
        index_to_erase.push_back(i);
      }
    }
    for (int k = index_to_erase.size()-1; k >= 0; --k) {
      table_->erase(table_->begin() + index_to_erase.at(k));
    }
  } else if (common_synonyms->size() == 1) {  // one common
    int num_of_synonyms = other_table->at(0).size();
    std::vector<std::string> col_one = GetColumnVec(common_synonyms->at(0).first);
    std::vector<std::string> other_col_one = other_result_table.GetColumnVec(
        common_synonyms->at(0).first);

    int other_col_two_index;
    if (num_of_synonyms == 2) {
      other_col_two_index = (common_synonyms->at(0).second + 1) % 2;
      auto *other_synonyms_to_index = other_result_table.get_synonym_to_index();
      for (auto &it : *other_synonyms_to_index) {
        if (std::find(synonyms_->begin(), synonyms_->end(), it.first)
        == synonyms_->end()) {  // if item is new
          synonym_to_index_->insert({it.first, synonym_to_index_->size()});
          index_to_synonym_->insert({index_to_synonym_->size(), it.first});
          synonyms_->push_back(it.first);
        }
      }
    }

    table *new_table = new std::vector<std::vector<std::string>>();
    std::vector<int> index_to_erase;
    for (int i = 0; i < col_one.size(); ++i) {
      bool found_matching_row = false;
      for (int j = 0; j < other_col_one.size(); ++j) {
        if (col_one.at(i) == other_col_one.at(j)) {
          if (num_of_synonyms == 1) {
            found_matching_row = true;
            break;
          } else {  // two synonyms
            std::vector<std::string> curr_vec = table_->at(i);
            curr_vec.push_back(other_table->at(j).at(other_col_two_index));
            new_table->push_back(curr_vec);
          }
        }
      }
      if (num_of_synonyms == 1 && !found_matching_row) {
        index_to_erase.push_back(i);
      }
    }
    if (num_of_synonyms != 1) {
      delete table_;
      table_ = new_table;
    }
    for (int k = index_to_erase.size()-1; k >= 0; --k) {
      table_->erase(table_->begin()+index_to_erase.at(k));
    }
  } else if (common_synonyms->size() == 0) {  // happens for cases when no used synonyms in clause group
    CrossJoin(other_result_table, {});
  }
}

void ResultTable::CrossJoin(ResultTable &other_result_table,
                            std::unordered_set<std::string> used_synonyms) {
  table *new_table = new std::vector<std::vector<std::string>>();
  table *other_table = other_result_table.get_table();
  if (other_table->empty()) {
    delete table_;
    table_ = new_table;
    return;
  }

  std::unordered_map<int, std::string> *other_index_to_synonym;
  other_index_to_synonym = other_result_table.get_index_to_synonym();
  std::vector<int> indexes_of_used_synonyms;
  for (int i = 0; i < other_index_to_synonym->size(); ++i) {
    std::string synonym = other_index_to_synonym->at(i);
    // if no used synonyms, add everything (case where clause groups are all not used)
    // else only add used synonyms into the table
    if (used_synonyms.empty() || used_synonyms.find(synonym) != used_synonyms.end()) {
      synonym_to_index_->insert({synonym, synonym_to_index_->size()});
      index_to_synonym_->insert({index_to_synonym_->size(), synonym});
      synonyms_->push_back(synonym);
      indexes_of_used_synonyms.push_back(i);
    }
  }

  for (int i = 0; i < table_->size(); ++i) {
    for (int j = 0; j < other_table->size(); ++j) {
      std::vector<std::string> curr_row = table_->at(i);

      // only add to table synonyms that will be used subsequently
      for (int k = 0; k < indexes_of_used_synonyms.size(); ++k) {
        curr_row.push_back(other_table->at(j).at(indexes_of_used_synonyms.at(k)));
      }
//      curr_row.insert(curr_row.end(), other_table->at(j).begin(), other_table->at(j).end());
      new_table->push_back(curr_row);
    }
  }
  delete table_;
  table_ = new_table;
}

// maximum of two synonyms due to the nature of the clauses
// returns pair of synonym and index in other result table
std::vector<std::pair<std::string, int>> *ResultTable::GetCommonSynonyms(
    ResultTable &other_result_table) {
  auto ret = new std::vector<std::pair<std::string, int>>();
  auto *other_synonyms_to_index = other_result_table.get_synonym_to_index();
  for (auto& it : *other_synonyms_to_index) {
    if (synonym_to_index_->find(it.first) != synonym_to_index_->end()) {
      ret->push_back(std::make_pair(it.first, it.second));
    }
  }
  return ret;
}

void ResultTable::AddSingleColumn(std::string synonym, std::vector<std::string> vec) {
  if (!table_->empty() && table_->size() != vec.size()) {
    throw std::runtime_error("Column size does not match table size!");
  }

  synonym_to_index_->insert({synonym, synonym_to_index_->size()});
  index_to_synonym_->insert({index_to_synonym_->size(), synonym});
  synonyms_->push_back(synonym);
  bool empty_table = table_->empty();
  for (int i = 0; i < vec.size(); ++i) {
    if (!empty_table) {
      table_->at(i).push_back(vec.at(i));
    } else {
      table_->push_back(std::vector<std::string>({vec.at(i)}));
    }
  }
}

void ResultTable::AddDoubleColumns(std::string synonym_one, std::vector<std::string> vec_one,
                                   std::string synonym_two, std::vector<std::string> vec_two) {
  if (!table_->empty() && (table_->size() != vec_one.size() || table_->size() != vec_two.size())) {
    throw std::runtime_error("Column size does not match table size!");
  }
  synonym_to_index_->insert({synonym_one, synonym_to_index_->size()});
  synonym_to_index_->insert({synonym_two, synonym_to_index_->size()});
  index_to_synonym_->insert({index_to_synonym_->size(), synonym_one});
  index_to_synonym_->insert({index_to_synonym_->size(), synonym_two});
  synonyms_->push_back(synonym_one);
  synonyms_->push_back(synonym_two);
  bool empty_table = table_->empty();
  for (int i = 0; i < vec_one.size() && i < vec_two.size(); ++i) {
    if (!empty_table) {
      table_->at(i).push_back(vec_one.at(i));
      table_->at(i).push_back(vec_two.at(i));
    } else {
      table_->push_back(std::vector<std::string>{vec_one.at(i), vec_two.at(i)});
    }
  }
}

std::vector<std::string> ResultTable::GetColumnVec(std::string synonym) {
  if (synonym_to_index_->find(synonym) == synonym_to_index_->end()) {
    throw std::runtime_error("Error finding synonym!");
  }
  int index = synonym_to_index_->at(synonym);
  std::vector<std::string> ret;
  for (std::vector<std::string> row : *table_) {
    ret.push_back(row.at(index));
  }
  return ret;
}

void ResultTable::set_table(ResultTable &result_table) {
  this->table_ = result_table.get_table();
  this->synonym_to_index_ = result_table.get_synonym_to_index();
  this->index_to_synonym_ = result_table.get_index_to_synonym();
  this->synonyms_ = result_table.get_synonyms();
}

table *ResultTable::get_table() {
  return this->table_;
}

std::unordered_map<std::string, int> *ResultTable::get_synonym_to_index() {
  return this->synonym_to_index_;
}

std::unordered_map<int, std::string> *ResultTable::get_index_to_synonym() {
  return this->index_to_synonym_;
}

std::vector<std::string> *ResultTable::get_synonyms() {
  return this->synonyms_;
}
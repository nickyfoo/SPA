#include "result_table.h"

ResultTable::ResultTable(std::vector<std::string> synonyms) {
  this->synonyms_ = synonyms;
  this->table_ = new std::vector<std::vector<std::string>>();
  this->synonym_to_index_ = new std::unordered_map<std::string, int>();
}

ResultTable::ResultTable() {
  this->table_ = new std::vector<std::vector<std::string>>();
  this->synonym_to_index_ = new std::unordered_map<std::string, int>();
}

void ResultTable::NaturalJoin(ResultTable &other_result_table) {
  auto common_synonyms = GetCommonSynonyms(other_result_table);
  table *other_table = other_result_table.get_table();
  if (common_synonyms->size() == 2) {  // two common
    std::vector<std::string> col_one = table_->at(synonym_to_index_->at(common_synonyms->at(0).first));
    std::vector<std::string> col_two = table_->at(synonym_to_index_->at(common_synonyms->at(1).first));
    std::vector<std::string> other_col_one = other_table->at(common_synonyms->at(0).second);
    std::vector<std::string> other_col_two = other_table->at(common_synonyms->at(1).second);

    for (int i = 0; i < col_one.size() && i < col_two.size(); ++i) {
        bool found_matching_row = false;
        for (int j = 0; j < other_col_one.size() && i < other_col_two.size(); ++j) {
          if (col_one.at(i) == other_col_one.at(j) && col_two.at(i) == other_col_two.at(j)) {
            found_matching_row = true;
            break;
          }
        }
        if (!found_matching_row) {
          table_->erase(table_->begin()+i);
          i--;
        }
    }
  } else if (common_synonyms->size() == 1) {  // one common
    int num_of_synonyms = other_table->size();
    std::vector<std::string> col_one = table_->at(synonym_to_index_->at(common_synonyms->at(0).first));
    std::vector<std::string> other_col_one = other_table->at(common_synonyms->at(0).second);
    int other_col_two_index = (common_synonyms->at(0).second + 1) % 2;
    table *new_table = new std::vector<std::vector<std::string>>();
    for (int i = 0; i < col_one.size(); ++i) {
      bool found_matching_row = false;
      for (int j = 0; j < other_col_one.size(); ++j) {
        if (col_one.at(i) == other_col_one.at(j)) {
          if (num_of_synonyms == 1) {
            found_matching_row = true;
            break;
          } else {  // two synonyms
            std::vector<std::string> curr_vec = table_->at(i);
            curr_vec.push_back(other_table->at(other_col_two_index).at(j));
            new_table->push_back(curr_vec);
          }
        }
      }
      if (num_of_synonyms == 1 && !found_matching_row) {
        table_->erase(table_->begin()+i);
        i--;
      }
    }
    if (num_of_synonyms != 1) {
      delete table_;
      table_ = new_table;
    }
  } else if (common_synonyms->size() == 0) { // should not happen
    throw std::runtime_error("Error on natural join.");
//    CrossJoin(other_result_table);
  }
}


void ResultTable::CrossJoin(ResultTable &other_result_table) {
  std::unordered_map<std::string, int> *other_synonyms_to_index = other_result_table.get_synonym_to_index();

  bool found_zero = false;
  for (auto& it : *other_synonyms_to_index) {
    if (other_synonyms_to_index->size() == 1) {
      synonym_to_index_->insert({it.first, synonym_to_index_->size()});
    } else {
      if ((it.second == 0 && !found_zero) || (it.second == 1 && found_zero)) {
        found_zero = true;
        synonym_to_index_->insert({it.first, synonym_to_index_->size()});
      } else if (it.second == 1 && !found_zero){
        synonym_to_index_->insert({it.first, synonym_to_index_->size()+1});
      } else if (it.second == 0 && found_zero){
        synonym_to_index_->insert({it.first, synonym_to_index_->size()-1});
      }
    }
  }

  table *other_table = other_result_table.get_table();
  table *new_table = new std::vector<std::vector<std::string>>();
  other_result_table.get_synonym_to_index();
  if (other_table->size() == 0) {
    delete table_;
    table_ = new_table;
    return;
  }

  for (int i = 0; i < table_->size(); ++i) {
    std::vector<std::string> curr_row = table_->at(i);
    for (int j = 0; j < other_table->size(); ++j) {
      for (int k = 0; k < other_table->at(j).size(); ++k) {
        curr_row.push_back(other_table->at(j).at(k));
      }
    }
    new_table->push_back(curr_row);
  }
  delete table_;
  table_ = new_table;
}

// maximum of two synonyms due to the nature of the clauses
std::vector<std::pair<std::string, int>> *ResultTable::GetCommonSynonyms(ResultTable &other_result_table) {
  auto ret = new std::vector<std::pair<std::string, int>>();
  std::unordered_map<std::string, int> *other_synonyms_to_index = other_result_table.get_synonym_to_index();
  for (auto& it : *other_synonyms_to_index) {
    if (synonym_to_index_->find(it.first) != synonym_to_index_->end()) {
      ret->push_back(std::make_pair(it.first, it.second));
    } else {
      synonym_to_index_->insert({it.first, synonym_to_index_->size()});
    }
  }
  return ret;
}

void ResultTable::set_table(ResultTable &result_table) {
  this->table_ = result_table.get_table();
  this->synonym_to_index_ = result_table.get_synonym_to_index();
}

table* ResultTable::get_table() {
  return this->table_;
}

std::unordered_map<std::string, int> *ResultTable::get_synonym_to_index() {
  return this->synonym_to_index_;
}
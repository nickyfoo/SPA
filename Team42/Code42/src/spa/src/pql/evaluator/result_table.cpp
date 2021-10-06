#include "result_table.h"

//ResultTable::ResultTable(std::vector<std::string> synonyms) {
//  this->synonyms_ = synonyms;
//  this->table_ = new std::vector<std::vector<std::string>>();
//  this->synonym_to_index_ = new std::unordered_map<std::string, int>();
//}

ResultTable::ResultTable() {
  this->table_ = new std::vector<std::vector<std::string>>();
  this->synonyms_ = new std::vector<std::string>();
  this->synonym_to_index_ = new std::unordered_map<std::string, int>();
  this->index_to_synonym_ = new std::unordered_map<int, std::string>();
}

void ResultTable::NaturalJoin(ResultTable &other_result_table) {
  auto common_synonyms = GetCommonSynonyms(other_result_table);
  table *other_table = other_result_table.get_table();
  printf("PRINT MY TABLE\n");
  for (std::vector<std::string> vec : *table_) {
    for (std::string s : vec) {
      printf("%s ", s.c_str());
    }
    printf("\n");
  }
  printf("PRINT oTHER TABLE\n");
  for (std::vector<std::string> vec : *other_table) {
    for (std::string s : vec) {
      printf("%s ", s.c_str());
    }
    printf("\n");
  }
  if (common_synonyms->size() == 2) {  // two common
    std::vector<std::string> col_one = GetColumnVec(common_synonyms->at(0).first);
    std::vector<std::string> col_two = GetColumnVec(common_synonyms->at(1).first);
    std::vector<std::string> other_col_one = other_result_table.GetColumnVec(common_synonyms->at(0).first);
    std::vector<std::string> other_col_two = other_result_table.GetColumnVec(common_synonyms->at(1).first);

    printf("\n");
    for (std::string s : col_one) {
      printf("%s ", s.c_str());
    }
    printf("\n");
    for (std::string s : col_two) {
      printf("%s ", s.c_str());
    }
    printf("\n");
    for (std::string s : other_col_one) {
      printf("%s ", s.c_str());
    }
    printf("\n");
    for (std::string s : other_col_two) {
      printf("%s ", s.c_str());
    }

    std::vector<int> index_to_erase;
    for (int i = 0; i < col_one.size() && i < col_two.size(); ++i) {
        bool found_matching_row = false;
        for (int j = 0; j < other_col_one.size() && j < other_col_two.size(); ++j) {
//          printf("got reach here2\n");
          printf("col one: %s\n", col_one.at(i).c_str());
          printf("col two: %s\n", col_two.at(i).c_str());
          printf("other col one: %s\n", other_col_one.at(j).c_str());
          printf("other col two: %s\n", other_col_two.at(j).c_str());
          if (col_one.at(i) == other_col_one.at(j) && col_two.at(i) == other_col_two.at(j)) {
            printf("got something heree\n");
            found_matching_row = true;
            break;
          }
        }
        if (!found_matching_row) {
//          printf("got reach here3\n");
//          printf("what is i here: %d\n", i);
//          table_->erase(table_->begin()+i);
//          printf("DIDNT FIND THIS\n");
//          printf("finished 3\n");
          index_to_erase.push_back(i);
//          i--;
        }
    }
    for (int k = index_to_erase.size()-1; k >= 0; --k) {
      table_->erase(table_->begin()+index_to_erase.at(k));
    }
  } else if (common_synonyms->size() == 1) {  // one common
    printf("MIHEREYO\n");
    int num_of_synonyms = other_table->at(0).size();
    printf("WHAT THE FUCK IS THE OTHER TABLE: \n");
    for (std::vector<std::string> vec : *other_table) {
      for (std::string s : vec) {
        printf("%s ", s.c_str());
      }
      printf("\n");
    }
    printf("num of synonym 1: %d\n", num_of_synonyms);
    std::vector<std::string> col_one = GetColumnVec(common_synonyms->at(0).first);
    printf("num of synonym 2: %d\n", num_of_synonyms);
//    std::vector<std::string> col_one = table_->at(synonym_to_index_->at(common_synonyms->at(0).first));
    for (std::string s : col_one) {
      printf("%s ", s.c_str());
    }
    printf("\n");
    printf("num of synonym 3: %d\n", num_of_synonyms);
//    std::vector<std::string> other_col_one = other_table->at(common_synonyms->at(0).second);
    std::vector<std::string> other_col_one = other_result_table.GetColumnVec(common_synonyms->at(0).first);
    for (std::string s : other_col_one) {
      printf("%s ", s.c_str());
    }
    printf("num of synonym 4: %d\n", num_of_synonyms);
    printf("\n");
    int other_col_two_index;
    printf("NUM OF SYNONYMS: %d\n", num_of_synonyms);
    if (num_of_synonyms == 2) {
      printf("DID IT EVEN COME HERE NO AH\n");
      other_col_two_index = (common_synonyms->at(0).second + 1) % 2;
//      std::unordered_map<std::string, int> *other_synonyms_to_index = other_result_table.get_synonym_to_index();
//      for (auto& it : *other_synonyms_to_index) {
//        if (synonym_to_index_->find(it.first) != synonym_to_index_->end()) {
//          synonym_to_index_->insert({it.first, synonym_to_index_->size()});
//          printf("other synonym is: %s\n", it.first.c_str());
//        } else {
//          printf("wtf is this: %s\n", it.first.c_str());
//        }
//      }
    }
    printf("other col two index: %d\n", other_col_two_index);
    table *new_table = new std::vector<std::vector<std::string>>();
    std::vector<int> index_to_erase;
    for (int i = 0; i < col_one.size(); ++i) {
      printf("SIZE OF COL ONE: %d\n", col_one.size());
      bool found_matching_row = false;
      for (int j = 0; j < other_col_one.size(); ++j) {
        printf("col_one.at(i): %s\n", col_one.at(i).c_str());
        printf("other col one : %s\n", other_col_one.at(j).c_str());
        if (col_one.at(i) == other_col_one.at(j)) {
          printf("found match\n");
          if (num_of_synonyms == 1) {
            printf("GOT ONE SYNONYM\n");
            found_matching_row = true;
            break;
          } else {  // two synonyms
            printf("GOT TWO SYNONYMS\n");
            printf("here1\n");
            std::vector<std::string> curr_vec = table_->at(i);
            printf("here2\n");
            curr_vec.push_back(other_table->at(j).at(other_col_two_index));
            printf("here3\n");
            new_table->push_back(curr_vec);
            printf("here4\n");
          }
        }
      }
      if (num_of_synonyms == 1 && !found_matching_row) {
        printf("erasing something\n");
//        printf("table entry to remove: %s\n", ta);
//        table_->erase(table_->begin()+i);
//        i--;
        index_to_erase.push_back(i);
      }
      printf("moving on....\n");
    }
    if (num_of_synonyms != 1) {
      delete table_;
      table_ = new_table;
    }
    for (int k = index_to_erase.size()-1; k >= 0; --k) {
      table_->erase(table_->begin()+index_to_erase.at(k));
    }
  } else if (common_synonyms->size() == 0) { // should not happen
//    throw std::runtime_error("Error on natural join.");
    CrossJoin(other_result_table);
  }
}

void ResultTable::CrossJoin(ResultTable &other_result_table) {
  std::unordered_map<int, std::string> *other_index_to_synonym = other_result_table.get_index_to_synonym();
  for (int i = 0; i < other_index_to_synonym->size(); ++i) {
    std::string synonym = other_index_to_synonym->at(i);
    synonym_to_index_->insert({synonym, synonym_to_index_->size()});
    index_to_synonym_->insert({index_to_synonym_->size(), synonym});
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
    for (int j = 0; j < other_table->size(); ++j) {
      std::vector<std::string> curr_row = table_->at(i);
      for (int k = 0; k < other_table->at(j).size(); ++k) {
        curr_row.push_back(other_table->at(j).at(k));
      }
      new_table->push_back(curr_row);
    }
  }
  delete table_;
  table_ = new_table;
}

//void ResultTable::CrossJoin(ResultTable &other_result_table) {
//  std::unordered_map<std::string, int> *other_synonyms_to_index = other_result_table.get_synonym_to_index();
//
//  bool found_zero = false;
//  for (auto& it : *other_synonyms_to_index) {
//    if (other_synonyms_to_index->size() == 1) {
//      synonym_to_index_->insert({it.first, synonym_to_index_->size()});
//      synonyms_->push_back(it.first);
//    } else {
//      if ((it.second == 0 && !found_zero) || (it.second == 1 && found_zero)) {
//        found_zero = true;
//        synonym_to_index_->insert({it.first, synonym_to_index_->size()});
//        synonyms_->push_back(it.first);
//      } else if (it.second == 1 && !found_zero){
//        synonym_to_index_->insert({it.first, synonym_to_index_->size()+1});
//        synonyms_->push_back(it.first);
//      } else if (it.second == 0 && found_zero){
//        synonym_to_index_->insert({it.first, synonym_to_index_->size()-1});
//        synonyms_->push_back(it.first);
//      }
//    }
//  }
//
//  table *other_table = other_result_table.get_table();
//  table *new_table = new std::vector<std::vector<std::string>>();
//  other_result_table.get_synonym_to_index();
//  if (other_table->size() == 0) {
//    delete table_;
//    table_ = new_table;
//    return;
//  }
//
//  for (int i = 0; i < table_->size(); ++i) {
//    std::vector<std::string> curr_row = table_->at(i);
//    for (int j = 0; j < other_table->size(); ++j) {
//      for (int k = 0; k < other_table->at(j).size(); ++k) {
//        curr_row.push_back(other_table->at(j).at(k));
//      }
//    }
//    new_table->push_back(curr_row);
//  }
//  delete table_;
//  table_ = new_table;
//}

// maximum of two synonyms due to the nature of the clauses
// returns pair of synonym and index in other result table
std::vector<std::pair<std::string, int>> *ResultTable::GetCommonSynonyms(ResultTable &other_result_table) {
  auto ret = new std::vector<std::pair<std::string, int>>();
  std::unordered_map<std::string, int> *other_synonyms_to_index = other_result_table.get_synonym_to_index();
  for (auto& it : *other_synonyms_to_index) {
    if (synonym_to_index_->find(it.first) != synonym_to_index_->end()) {
      ret->push_back(std::make_pair(it.first, it.second));
    } else {
      synonym_to_index_->insert({it.first, synonym_to_index_->size()});
      index_to_synonym_->insert({index_to_synonym_->size(), it.first});
      synonyms_->push_back(it.first);
    }
  }
  return ret;
}

void ResultTable::AddSingleColumn(std::string synonym, std::vector<std::string> vec) {
  printf("vec size outside: %d\n", vec.size());
  if (!table_->empty() && table_->size() != vec.size()) {
    printf("TABLE AT 0 SIZE: %d\n", table_->at(0).size());
    printf("VEC SIZE: %d\n", vec.size());
    throw std::runtime_error("Column size does not match table size!");
  }
  printf("gottt not\n");
  synonym_to_index_->insert({synonym, synonym_to_index_->size()});
  index_to_synonym_->insert({index_to_synonym_->size(), synonym});
  bool empty_table = table_->empty();
  for (int i = 0; i < vec.size(); ++i) {
    if (!empty_table) {
      table_->at(i).push_back(vec.at(i));
    } else {
      printf("HEREE\n");
      printf("vec value is : %s\n", vec.at(i).c_str());
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
  bool empty_table = table_->empty();
  for (int i = 0; i < vec_one.size() && i < vec_two.size(); ++i) {
    if (!empty_table) {
      table_->at(i).push_back(vec_one.at(i));
      table_->at(i).push_back(vec_two.at(i));
    } else {
      printf("HEREE\n");
      table_->push_back(std::vector<std::string>{vec_one.at(i), vec_two.at(i)});
    }
  }
//  for (std::string value_one : vec_one) {
//    for (std::string value_two : vec_two) {
//      printf("value one: %s\n", value_one.c_str());
//      printf("value two: %s\n", value_two.c_str());
//      table_->push_back(std::vector<std::string>({value_one, value_two}));
//    }
//  }
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
}

table* ResultTable::get_table() {
  return this->table_;
}

std::unordered_map<std::string, int> *ResultTable::get_synonym_to_index() {
  return this->synonym_to_index_;
}

std::unordered_map<int, std::string> *ResultTable::get_index_to_synonym() {
  return this->index_to_synonym_;
}
